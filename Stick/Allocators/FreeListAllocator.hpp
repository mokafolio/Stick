#ifndef STICK_ALLOCATORS_FREELISTALLOCATOR_HPP
#define STICK_ALLOCATORS_FREELISTALLOCATOR_HPP

#include <Stick/Allocators/MemoryChunk.hpp>

namespace stick
{
namespace mem
{
template <class Alloc, Size S>
class STICK_API FreeListAllocator
{
  public:
    static constexpr Size alignment = Alloc::alignment;

    using ParentAllocator = Alloc;

    // static_assert(S > sizeof(FreeBlock), "The memory is too small.")

    inline FreeListAllocator() : m_freeList(nullptr)
    {
        // m_memory = m_alloc.allocate(S, alignment);
        // STICK_ASSERT(m_memory);
        // deallocateAll();
    }

    inline ~FreeListAllocator()
    {
        // m_alloc.deallocate(m_memory);

        // iterate over all allocater blocks and deallocate them
        if (m_firstBlock.memory)
        {
            static constexpr Size headerSize = sizeof(MemoryChunk);
            static constexpr Size headerAdjustment =
                headerSize % alignment == 0
                    ? headerSize
                    : headerSize + headerSize + alignment - headerSize % alignment;
            MemoryChunk * pb = &m_firstBlock;
            while (pb)
            {
                MemoryChunk * tmp = pb->next;
                m_alloc.deallocate({ (void *)((UPtr)pb->memory.ptr - headerAdjustment),
                                     pb->memory.size + headerAdjustment });
                pb = tmp;
            }
        }
    }

    inline Block allocate(Size _byteCount, Size _alignment)
    {
        STICK_ASSERT(_byteCount > 0);

        if (_byteCount + sizeof(AllocationHeader) > S)
        {
            return Block();
        }

        FreeBlock * prevBlock = nullptr;
        FreeBlock * currentBlock = m_freeList;

        while (currentBlock != nullptr)
        {
            Size adjustment =
                alignmentAdjustmentWithHeader(currentBlock, _alignment, sizeof(AllocationHeader));
            Size totalSize = adjustment + _byteCount;

            if (currentBlock->size >= totalSize)
            {
                auto diff = currentBlock->size - totalSize;
                if (diff > sizeof(AllocationHeader))
                {
                    FreeBlock * nextBlock = reinterpret_cast<FreeBlock *>(
                        reinterpret_cast<UPtr>(currentBlock) + totalSize);
                    nextBlock->size = currentBlock->size - totalSize;
                    nextBlock->next = currentBlock->next;

                    if (prevBlock)
                        prevBlock->next = nextBlock;
                    else
                        m_freeList = nextBlock;
                }
                else
                {
                    // if the remaining space is too small, we add it to the current block
                    totalSize = currentBlock->size;

                    if (prevBlock)
                        prevBlock->next = currentBlock->next;
                    else
                        m_freeList = currentBlock->next;
                }

                UPtr alignedAddr = reinterpret_cast<UPtr>(currentBlock) + adjustment;

                AllocationHeader * header =
                    reinterpret_cast<AllocationHeader *>(alignedAddr - sizeof(AllocationHeader));
                header->size = totalSize;
                header->adjustment = adjustment;

                STICK_ASSERT(owns({ reinterpret_cast<void *>(alignedAddr), _byteCount }));

                return { reinterpret_cast<void *>(alignedAddr), _byteCount };
            }
            else
            {
                prevBlock = currentBlock;
                currentBlock = currentBlock->next;
            }
        }

        allocateChunk();
        return allocate(_byteCount, _alignment);
    }

    inline bool owns(const Block & _blk) const
    {
        const MemoryChunk * pb = &m_firstBlock;
        while (pb)
        {
            auto ret = pb->owns(_blk);
            if (ret)
                return ret;
            pb = pb->next;
        }
        return false;
    }

    inline void deallocate(const Block & _blk)
    {
        STICK_ASSERT(owns(_blk));

        UPtr hdr = reinterpret_cast<UPtr>(_blk.ptr) - sizeof(AllocationHeader);
        AllocationHeader * header = reinterpret_cast<AllocationHeader *>(hdr);

        FreeBlock * prevBlock = nullptr;
        FreeBlock * currentBlock = m_freeList;

        UPtr blockStart = reinterpret_cast<UPtr>(_blk.ptr) - header->adjustment;
        Size blockSize = header->size;
        UPtr blockEnd = blockStart + blockSize;

        // try to find where in the list the block belongs
        while (currentBlock != nullptr)
        {
            if (reinterpret_cast<UPtr>(currentBlock) >= blockEnd)
                break;

            prevBlock = currentBlock;
            currentBlock = currentBlock->next;
        }

        // new first block
        if (prevBlock == nullptr)
        {
            prevBlock = reinterpret_cast<FreeBlock *>(blockStart);
            prevBlock->size = blockSize;
            prevBlock->next = currentBlock;
            m_freeList = prevBlock;
        }
        // adjacent block that we can seamlessly merge with the previous one
        else if (reinterpret_cast<UPtr>(prevBlock) + prevBlock->size == blockStart)
        {
            STICK_ASSERT(chunk({ prevBlock, prevBlock->size }) ==
                         chunk({ (void *)blockStart, blockSize }));
            prevBlock->size += blockSize;
        }
        // append to previous block
        else
        {
            FreeBlock * nextBlock = reinterpret_cast<FreeBlock *>(blockStart);
            nextBlock->size = blockSize;
            nextBlock->next = prevBlock->next;

            STICK_ASSERT((UPtr)nextBlock + nextBlock->size <= chunk(_blk)->memory.end());

            prevBlock->next = nextBlock;
            prevBlock = nextBlock;
        }

        if (currentBlock != nullptr && reinterpret_cast<UPtr>(currentBlock) == blockEnd)
        {
            STICK_ASSERT(chunk({ prevBlock, prevBlock->size }) ==
                         chunk({ currentBlock, currentBlock->size }));

            prevBlock->size += currentBlock->size;
            prevBlock->next = currentBlock->next;
        }
    }

    inline void deallocateAll()
    {
        // m_freeList = reinterpret_cast<FreeBlock *>(m_memory.ptr);
        // m_freeList->size = S;
        // m_freeList->next = nullptr;

        if (m_firstBlock.memory)
        {
            const MemoryChunk * pb = &m_firstBlock;
            m_freeList = reinterpret_cast<FreeBlock *>(m_firstBlock.memory.ptr);
            FreeBlock * blk = m_freeList;
            blk->size = m_firstBlock.memory.size;
            pb = pb->next;
            while (pb)
            {
                FreeBlock * block = reinterpret_cast<FreeBlock *>(pb->memory.ptr);
                block->size = pb->memory.size;
                blk->next = block;
                blk = block;
                pb = pb->next;
            }
            blk->next = nullptr;
        }
    }

    inline Size freeCount() const
    {
        Size ret = 0;
        FreeBlock * p = m_freeList;
        while (p)
        {
            p = p->next;
            ret++;
        }
        return ret;
    }

    inline Size chunkCount() const
    {
        Size ret = 0;
        auto * p = &m_firstBlock;
        while (p)
        {
            p = p->next;
            ret++;
        }
        return ret;
    }

  private:
    inline void allocateChunk()
    {
        static constexpr Size headerSize = sizeof(MemoryChunk);
        static constexpr Size headerAdjustment =
            headerSize % alignment == 0
                ? headerSize
                : headerSize + headerSize + alignment - headerSize % alignment;

        Size size = S + headerAdjustment;
        Block mem = m_alloc.allocate(size, alignment);
        MemoryChunk blk(
            { (void *)((UPtr)mem.ptr + headerAdjustment), mem.size - headerAdjustment });

        if (!m_firstBlock.memory)
        {
            m_firstBlock = std::move(blk);
        }
        else
        {
            MemoryChunk * last = m_firstBlock.lastChunk();
            last->next = reinterpret_cast<MemoryChunk *>((UPtr)blk.memory.ptr - headerAdjustment);
            *last->next = std::move(blk);
        }

        if (!m_freeList)
        {
            m_freeList = reinterpret_cast<FreeBlock *>(blk.memory.ptr);
            m_freeList->size = blk.memory.size;
            m_freeList->next = nullptr;
        }
        else
        {
            FreeBlock * fblk = reinterpret_cast<FreeBlock *>(blk.memory.ptr);
            FreeBlock * last = m_freeList;
            while (last->next)
            {
                last = last->next;
            }
            fblk->size = blk.memory.size;
            fblk->next = nullptr;
            last->next = fblk;
        }
    }

    inline const MemoryChunk * chunk(const Block & _blk) const
    {
        const MemoryChunk * pb = &m_firstBlock;
        while (pb)
        {
            if (pb->owns(_blk))
                return pb;
            pb = pb->next;
        }
        return nullptr;
    }

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

    ParentAllocator m_alloc;
    MemoryChunk m_firstBlock;
    FreeBlock * m_freeList;
};
} // namespace mem
} // namespace stick

#endif // STICK_ALLOCATORS_FREELISTALLOCATOR_HPP
