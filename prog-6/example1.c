int empty1(int a){
    return a+1;
}

void empty2(){
    empty1(1);
}

int empty3(){
    return 1;
}

int main(){
    return empty3();
}
