#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static bool print(const char* data, size_t len)
{
	const unsigned char* bytes = reinterpret_cast<const unsigned char*>(data);
	for(size_t i = 0; i < len; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

template<typename T>
static bool print_integer(T value, size_t& out_len)
{
	char buffer[32] {};
	int len = 0;
	bool sign = false;

	if (value < 0)
	{
		sign = true;
		buffer[len++] = ((value % 10 + 10) % 10) + '0';
		value = -(value / 10);
	}

	while (value)
	{
		buffer[len++] = (value % 10) + '0';
		value /= 10;
	}

	if (sign)
		buffer[len++] = '-';
	if (len == 0)
		buffer[len++] = '0';

	for (int i = 0; i < len / 2; i++)
	{
		char temp = buffer[i];
		buffer[i] = buffer[len - i - 1];
		buffer[len - i - 1] = temp;
	}

	if (!print(buffer, len))
		return false;

	out_len = len;
	return true;
}

static char bits_to_hex(int bits, bool upper_case)
{
	if (bits < 10)
		return bits + '0';
	return bits - 10 + (upper_case ? 'A' : 'a');
}

template<typename T>
static bool print_hex(T value, bool upper_case, size_t& out_len)
{
	char buffer[16] {};
	int len = 0;

	while (value)
	{
		buffer[len++] = bits_to_hex(value & 0xF, upper_case);
		value >>= 4;
	}

	if (len == 0)
		buffer[len++] = '0';

	for (int i = 0; i < len / 2; i++)
	{
		char temp = buffer[i];
		buffer[i] = buffer[len - i - 1];
		buffer[len - i - 1] = temp;
	}

	if (!print(buffer, len))
		return false;

	out_len = len;
	return true;
}

int printf(const char* __restrict format, ...)
{
	va_list args;
	va_start(args, format);

	int written = 0;

	while (*format)
	{
		size_t max_rem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%')
		{
			if (format[0] == '%')
				format++;
			size_t len = 1;
			while (format[len] && format[len] != '%')
				len++;
			if (!print(format, len))
				return -1;
			format += len;
			written += len;
			continue;
		}

		const char* format_start = format++;

		if (*format == 'c')
		{
			format++;
			char c = (char)va_arg(args, int);
			if (!max_rem) // FIXME: EOVERFLOW
				return -1;
			if (!print(&c, sizeof(c)))
				return -1;
			written++;
		}
		else if (*format == 's')
		{
			format++;
			const char* str = va_arg(args, const char*);
			size_t len = strlen(str);
			if (max_rem < len) // FIXME: EOVERFLOW
				return -1;
			if (!print(str, len))
				return -1;
			written += len;
		}
		else if (*format == 'd' || *format == 'i')
		{
			format++;
			int value = va_arg(args, int);
			size_t len;
			if (!print_integer<int>(value, len))
				return -1;
			written += len;
		}
		else if (*format == 'u')
		{
			format++;
			unsigned int value = va_arg(args, unsigned int);
			size_t len;
			if (!print_integer<unsigned int>(value, len))
				return -1;
			written += len;
		}
		else if (*format == 'x' || *format == 'X')
		{
			format++;
			unsigned int value = va_arg(args, unsigned int);
			size_t len;
			if (!print_hex<unsigned int>(value, *(format - 1) == 'X', len))
				return -1;
			written += len;
		}
		else if (*format == 'p')
		{
			format++;
			void* ptr = va_arg(args, void*);
			size_t len;
			if (!print("0x", 2) || !print_hex<ptrdiff_t>((ptrdiff_t)ptr, false, len))
				return -1;
			written += len;
		}
		else
		{
			format = format_start;
			size_t len = strlen(format);
			if (max_rem < len) // FIXME: EOVERFLOW
				return -1;
			if (!print(format, len))
				return -1;
			written += len;
			format += len;
		}
	}

	va_end(args);
	return written;
}
