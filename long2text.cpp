void long2text(unsigned long val, char *buffer)
{
    buffer[0] = '0';
    if (val >= 50000000)
    {
        buffer[0] += 5;
        val -= 50000000;
    }
    if (val >= 30000000)
    {
        buffer[0] += 3;
        val -= 30000000;
    }
    if (val >= 20000000)
    {
        buffer[0] += 2;
        val -= 20000000;
    }
    if (val >= 10000000)
    {
        buffer[0] += 1;
        val -= 10000000;
    }

    buffer[1] = '0';
    if (val >= 5000000)
    {
        buffer[1] += 5;
        val -= 5000000;
    }
    if (val >= 3000000)
    {
        buffer[1] += 3;
        val -= 3000000;
    }
    if (val >= 2000000)
    {
        buffer[1] += 2;
        val -= 2000000;
    }
    if (val >= 1000000)
    {
        buffer[1] += 1;
        val -= 1000000;
    }

    buffer[2] = '0';
    if (val >= 500000)
    {
        buffer[2] += 5;
        val -= 500000;
    }
    if (val >= 300000)
    {
        buffer[2] += 3;
        val -= 300000;
    }
    if (val >= 200000)
    {
        buffer[2] += 2;
        val -= 200000;
    }
    if (val >= 100000)
    {
        buffer[2] += 1;
        val -= 100000;
    }

    buffer[3] = '0';
    if (val >= 50000)
    {
        buffer[3] += 5;
        val -= 50000;
    }
    register unsigned val2 = (unsigned)val;
    if (val2 >= 30000)
    {
        buffer[3] += 3;
        val2 -= 30000;
    }
    if (val2 >= 20000)
    {
        buffer[3] += 2;
        val2 -= 20000;
    }
    if (val2 >= 10000)
    {
        buffer[3] += 1;
        val2 -= 10000;
    }

    buffer[4] = '0';
    if (val2 >= 5000)
    {
        buffer[4] += 5;
        val2 -= 5000;
    }
    if (val2 >= 3000)
    {
        buffer[4] += 3;
        val2 -= 3000;
    }
    if (val2 >= 2000)
    {
        buffer[4] += 2;
        val2 -= 2000;
    }
    if (val2 >= 1000)
    {
        buffer[4] += 1;
        val2 -= 1000;
    }

    buffer[5] = '0';
    if (val2 >= 500)
    {
        buffer[5] += 5;
        val2 -= 500;
    }
    if (val2 >= 300)
    {
        buffer[5] += 3;
        val2 -= 300;
    }
    if (val2 >= 200)
    {
        buffer[5] += 2;
        val2 -= 200;
    }
    if (val2 >= 100)
    {
        buffer[5] += 1;
        val2 -= 100;
    }

    buffer[6] = '0';
    if (val2 >= 50)
    {
        buffer[6] += 5;
        val2 -= 50;
    }
    if (val2 >= 30)
    {
        buffer[6] += 3;
        val2 -= 30;
    }
    if (val2 >= 20)
    {
        buffer[6] += 2;
        val2 -= 20;
    }
    if (val2 >= 10)
    {
        buffer[6] += 1;
        val2 -= 10;
    }

    buffer[7] = '0';
    if (val2 >= 5)
    {
        buffer[7] += 5;
        val2 -= 5;
    }
    if (val2 >= 3)
    {
        buffer[7] += 3;
        val2 -= 3;
    }
    if (val2 >= 2)
    {
        buffer[7] += 2;
        val2 -= 2;
    }
    if (val2 >= 1)
    {
        buffer[7] += 1;
        val2 -= 1;
    }

    buffer[8] = '\0';

    if (buffer[0] == '0')
        buffer[0] = ' ';
    else
        return;
    if (buffer[1] == '0')
        buffer[1] = ' ';
    else
        return;
    if (buffer[2] == '0')
        buffer[2] = ' ';
    else
        return;
    if (buffer[3] == '0')
        buffer[3] = ' ';
    else
        return;
    if (buffer[4] == '0')
        buffer[4] = ' ';
    else
        return;
    if (buffer[5] == '0')
        buffer[5] = ' ';
    else
        return;
    if (buffer[6] == '0')
        buffer[6] = ' ';
    else
        return;
}
