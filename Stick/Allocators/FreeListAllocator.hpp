#ifndef STICK_ALLOCATORS_FREELISTALLOCATOR_HPP
#define STICK_ALLOCATORS_FREELISTALLOCATOR_HPP

#include <Stick/Allocators/Block.hpp>

namespace stick
{
    namespace mem
    {
        template<class Alloc, Size S>
        class STICK_API FreeListAllocator
        {
        public:

            static constexpr Size alignment = Alloc::alignment;

            inline FreeListAllocator(Alloc & _parentAllocator) :
                m_parentAllocator(&_parentAllocator)
            {
                m_memory = _parentAllocator.allocate(S, alignment);
                STICK_ASSERT(m_memory);
                deallocateAll();
            }

            inline ~FreeListAllocator()
            {
                m_parentAllocator->deallocate(m_memory);
            }

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                STICK_ASSERT(_byteCount > 0);

                FreeBlock * prevBlock = nullptr;
                FreeBlock * currentBlock = m_freeList;

                while (currentBlock != nullptr)
                {
                    Size adjustment = alignmentAdjustmentWithHeader(currentBlock, _alignment, sizeof(AllocationHeader));
                    Size totalSize = adjustment + _byteCount;

                    if (currentBlock->size >= totalSize)
                    {
                        if (currentBlock->size - totalSize > sizeof(AllocationHeader))
                        {
                            FreeBlock * nextBlock = reinterpret_cast<FreeBlock *>(reinterpret_cast<UPtr>(currentBlock) + totalSize);
                            nextBlock->size = currentBlock->size - totalSize;
                            nextBlock->next = currentBlock->next;

                            if (prevBlock)
                                prevBlock->next = nextBlock;
                            else
                                m_freeList = nextBlock;
                        }
                        else
                        {
                            //if the remaining space is too small, we add it to the current block
                            totalSize = currentBlock->size;

                            if (prevBlock)
                                prevBlock->next = currentBlock->next;
                            else
                                m_freeList = currentBlock->next;
                        }

                        UPtr alignedAddr = reinterpret_cast<UPtr>(currentBlock) + adjustment;

                        AllocationHeader * header = reinterpret_cast<AllocationHeader *>(alignedAddr - sizeof(AllocationHeader));
                        header->size = totalSize;
                        header->adjustment = adjustment;

                        printf("ADJUSTMENT %lu %lu %lu %lu\n", adjustment, totalSize, alignedAddr, (UPtr)m_memory.ptr);

                        return {reinterpret_cast<void *>(alignedAddr), _byteCount};
                    }
                    else
                    {
                        prevBlock = currentBlock;
                        currentBlock = currentBlock->next;
                    }
                }

                return {nullptr, 0};
            }

            inline bool owns(const Block & _blk) const
            {
                UPtr start = reinterpret_cast<UPtr>(_blk.ptr);
                UPtr start2 = reinterpret_cast<UPtr>(m_memory.ptr);
                return start >= start2 && start <= start2 +  m_memory.size;
            }

            inline void deallocate(const Block & _blk)
            {
                STICK_ASSERT(owns(_blk));

                UPtr hdr = reinterpret_cast<UPtr>(_blk.ptr) - sizeof(AllocationHeader);
                AllocationHeader * header = reinterpret_cast<AllocationHeader*>(hdr);

                FreeBlock * prevBlock = nullptr;
                FreeBlock * currentBlock = m_freeList;

                UPtr blockStart = reinterpret_cast<UPtr>(_blk.ptr) - header->adjustment;
                Size blockSize = header->size;
                UPtr blockEnd = blockStart + blockSize;

                //try to find where in the list the block belongs
                while (currentBlock != nullptr)
                {
                    if (reinterpret_cast<UPtr>(currentBlock) >= blockEnd)
                        break;

                    prevBlock = currentBlock;
                    currentBlock = currentBlock->next;
                }

                //new first block
                if (prevBlock == nullptr)
                {
                    printf("NEW FIRST BLOCK\n");
                    prevBlock = reinterpret_cast<FreeBlock *>(blockStart);
                    prevBlock->size = blockSize;
                    prevBlock->next = m_freeList;
                    m_freeList = prevBlock;
                }
                //adjacent block that we can seamlessly merge with the previous one
                else if (reinterpret_cast<UPtr>(prevBlock) + prevBlock->size == blockStart)
                {
                    printf("MERGE PREV BLOCK\n");
                    prevBlock->size += blockSize;
                }
                //append to previous block
                else
                {
                    printf("MERGE PREV BLOCK\n");
                    FreeBlock * nextBlock = reinterpret_cast<FreeBlock *>(blockEnd);
                    nextBlock->size = blockSize;
                    nextBlock->next = prevBlock->next;

                    prevBlock->next = nextBlock;
                    prevBlock = nextBlock;
                }

                if (currentBlock != nullptr && reinterpret_cast<UPtr>(currentBlock) == blockEnd)
                {
                    printf("MERGE LAST BLOCK\n");
                    prevBlock->size += currentBlock->size;
                    prevBlock->next = currentBlock->next;
                }
            }

            inline void deallocateAll()
            {
                m_freeList = reinterpret_cast<FreeBlock *>(m_memory.ptr);
                m_freeList->size = S;
                m_freeList->next = nullptr;
            }

            inline const Block & block() const
            {
                return m_memory;
            }

        private:

            struct FreeBlock
            {
                Size size;
                FreeBlock * next;
            };

            struct AllocationHeader
            {
                Size size;
                Size adjustment;
            };

            Alloc * m_parentAllocator;
            FreeBlock * m_freeList;
            Block m_memory;
        };
    }
}

#endif //STICK_ALLOCATORS_FREELISTALLOCATOR_HPP
