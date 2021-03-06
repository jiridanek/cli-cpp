/* 
 * File:   Printer.cpp
 * Author: opiske
 * 
 * Created on May 7, 2015, 9:48 AM
 */

#include "Printer.h"

Printer::Printer()
    : base(DECIMAL),
    stream(stdout)

{
}

Printer::Printer(FILE *stream)
    : base(DECIMAL),
    stream(stream)
{
}

Printer::Printer(const Printer& orig)
    : base(DECIMAL),
    stream(orig.stream)
{
}

Printer::~Printer()
{

}

void Printer::flush()
{
    fflush(stream);

}

void Printer::print(int val)
{
    switch (base) {
    case HEXADECIMAL:
    {

        fprintf(stream, "%x", val);
        break;
    }
    case OCTAL:
    {
        fprintf(stream, "%o", val);
        break;
    }
    case DECIMAL:
    default:
    {
        fprintf(stream, "%d", val);
        break;
    }
    }
}

void Printer::print(unsigned int val)
{
    switch (base) {
    case HEXADECIMAL:
    {
        fprintf(stream, "%#x", val);
        break;
    }
    case OCTAL:
    {
        fprintf(stream, "%#o", val);
        break;
    }
    case DECIMAL:
    default:
    {
        fprintf(stream, "%u", val);
        break;
    }
    }
}

void Printer::print(long int val)
{
    switch (base) {
    case HEXADECIMAL:
    {
        fprintf(stream, "%#lx", val);
        break;
    }
    case OCTAL:
    {
        fprintf(stream, "%#lo", val);
        break;
    }
    case DECIMAL:
    default:
    {
        fprintf(stream, "%ld", val);
        break;
    }
    }
}

void Printer::print(unsigned long int val)
{
    switch (base) {
    case HEXADECIMAL:
    {
        fprintf(stream, "%lx", val);
        break;
    }
    case OCTAL:
    {
        fprintf(stream, "%#lo", val);
        break;
    }
    case DECIMAL:
    default:
    {
        fprintf(stream, "%lu", val);
        break;
    }
    }
}

void Printer::print(double d)
{
    switch (base) {
    case HEXADECIMAL:
    {
        fprintf(stream, "#%a", d);
        break;
    }
    case OCTAL:
    {
        fprintf(stream, "%#lg", d);
        break;
    }
    case DECIMAL:
    default:
    {
        fprintf(stream, "%lg", d);
        break;
    }
    }
}

void Printer::print(char c)
{
    fprintf(stream, "%c", c);
}

void Printer::print(long long val)
{
    switch (base) {
    case HEXADECIMAL:
    {
        fprintf(stream, "%llx", val);
        break;
    }
    case OCTAL:
    {
        fprintf(stream, "%#llo", val);
        break;
    }
    case DECIMAL:
    default:
    {
        fprintf(stream, "%lld", val);
        break;
    }
    }
}

void Printer::print(const void *ptr)
{
    fprintf(stream, "%p", ptr);
}

void Printer::print(const char *str)
{
    fprintf(stream, "%s", str);
}
