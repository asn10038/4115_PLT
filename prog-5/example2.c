int sum(int a, int e){
    int res = 0;
    while (a < e){
        int b = a + 1;
        a = b*2;
    }
    return res+a;
}