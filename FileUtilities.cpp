#include <Stick/FileUtilities.hpp>
#include <stdio.h>

namespace stick
{
    Result<ByteArray> loadBinaryFile(const URI & _uri)
    {
        ByteArray ba;
        FILE * fp = fopen(toString(_uri).cString(), "rb");
        if (!fp)
        {
            return {ba, Error(ec::SystemErrorCode(errno), "fopen failed.", STICK_FILE, STICK_LINE)};
        }
        Size fsize;
        if (fseek(fp, 0, SEEK_END) < 0)
        {
            fclose(fp);
            return {ba, Error(ec::SystemErrorCode(errno), "fseek failed.", STICK_FILE, STICK_LINE)};
        }
        fsize = ftell(fp);
        if (fsize == EOF)
        {
            fclose(fp);
            return {ba, Error(ec::SystemErrorCode(errno), "ftell failed.", STICK_FILE, STICK_LINE)};
        }
        if (fseek(fp, 0, SEEK_SET) < 0)
        {
            fclose(fp);
            return {ba, Error(ec::SystemErrorCode(errno), "fseek failed.", STICK_FILE, STICK_LINE)};
        }

        ba.resize(fsize);
        fread(&ba[0], sizeof(char), fsize, fp);
        if (ferror(fp) != 0)
        {
            fclose(fp);
            return {ba, Error(ec::SystemErrorCode(errno), "fread failed.", STICK_FILE, STICK_LINE)};
        }

        fclose(fp);
        return {ba, Error()};
    }

    Result<String> loadTextFile(const URI & _uri)
    {
        String str;
        FILE * fp = fopen(toString(_uri).cString(), "r");
        if (!fp)
        {
            return {str, Error(ec::SystemErrorCode(errno), "fopen failed.", STICK_FILE, STICK_LINE)};
        }
        Size fsize, read;
        if (fseek(fp, 0, SEEK_END) < 0)
        {
            fclose(fp);
            return {str, Error(ec::SystemErrorCode(errno), "fseek failed.", STICK_FILE, STICK_LINE)};
        }
        fsize = ftell(fp);
        if (fsize == EOF)
        {
            fclose(fp);
            return {str, Error(ec::SystemErrorCode(errno), "ftell failed.", STICK_FILE, STICK_LINE)};
        }
        if (fseek(fp, 0, SEEK_SET) < 0)
        {
            fclose(fp);
            return {str, Error(ec::SystemErrorCode(errno), "fseek failed.", STICK_FILE, STICK_LINE)};
        }

        str.resize(fsize);
        fread(&str[0], sizeof(char), fsize, fp);
        if (ferror(fp) != 0)
        {
            fclose(fp);
            return {str, Error(ec::SystemErrorCode(errno), "fread failed.", STICK_FILE, STICK_LINE)};
        }

        fclose(fp);
        return {str, Error()};
    }

    Error saveBinaryFile(const ByteArray & _data, const URI & _uri)
    {
        String str;
        FILE * fp = fopen(toString(_uri).cString(), "wb");
        if (!fp)
        {
            return Error(ec::SystemErrorCode(errno), "fopen failed.", STICK_FILE, STICK_LINE);
        }

        fwrite(&_data[0], sizeof(char), _data.count(), fp);
        if (ferror(fp) != 0)
        {
            fclose(fp);
            return Error(ec::SystemErrorCode(errno), "fwrite failed.", STICK_FILE, STICK_LINE);
        };

        fclose(fp);
        return Error();
    }

    Error saveTextFile(const String & _text, const URI & _uri)
    {
        String str;
        FILE * fp = fopen(toString(_uri).cString(), "w");
        if (!fp)
        {
            return Error(ec::SystemErrorCode(errno), "fopen failed.", STICK_FILE, STICK_LINE);
        }

        fwrite(_text.cString(), sizeof(unsigned char), _text.length(), fp);
        if (ferror(fp) != 0)
        {
            fclose(fp);
            return Error(ec::SystemErrorCode(errno), "fwrite failed.", STICK_FILE, STICK_LINE);
        };

        fclose(fp);
        return Error();
    }
}
