{
    int i, j;
    String str1, str2;

    str1 = "Eu sou uma String";
    i = 2;

    while (i <= 10)
    {
        i = i + 2;
        print(i);
    }

    print("Variavel i agora e maior que 10.");
    print(i);

    print("Insira um inteiro:");
    read(j);
    print("Voce digitou:");
    print(j);

    print("Insira uma string:");
    read(str2);
    print("Voce digitou:");
    print(str2);

    if (i > 10 && j != 25)
    {
        print(":)");
    }

    print("Fim.");
}