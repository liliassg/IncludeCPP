SOURCE(pyutils.cpp) && HEADER(pyutils.h) pyutils

PUBLIC(
    pyutils CLASS(PyString) {
        METHOD(split)
        METHOD(join)
        METHOD(strip)
        METHOD(lstrip)
        METHOD(rstrip)
        METHOD(upper)
        METHOD(lower)
        METHOD(replace)
        METHOD(startswith)
        METHOD(endswith)
        METHOD(count)
        METHOD(find)
        METHOD(slice)
        METHOD(isdigit)
        METHOD(isalpha)
        METHOD(zfill)
    }

    pyutils CLASS(PyPath) {
        METHOD(join)
        METHOD(dirname)
        METHOD(basename)
        METHOD(abspath)
        METHOD(exists)
        METHOD(isfile)
        METHOD(isdir)
        METHOD(listdir)
        METHOD(makedirs)
        METHOD(remove)
        METHOD(extension)
        METHOD(getsize)
        METHOD(normpath)
    }

    pyutils CLASS(PyDict) {
        METHOD(keys)
        METHOD(values)
        METHOD(has_key)
        METHOD(get)
        METHOD(fromkeys)
    }

    pyutils CLASS(PyTime) {
        METHOD(strftime)
        METHOD(now)
        METHOD(timestamp)
        METHOD(sleep)
    }

    pyutils CLASS(PyFile) {
        METHOD(read)
        METHOD(readlines)
        METHOD(write)
        METHOD(writelines)
        METHOD(append)
    }
)
