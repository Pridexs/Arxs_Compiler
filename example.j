int max(int a, int b)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

{
    int i,j, biggest;

    print("Type number a:");
    read(i);
    print("Type number b:");
    read(j);

    biggest = max(i, j);
    print(biggest);
}