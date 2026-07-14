int main(){
    char* ptr=(char*)0xb8000;
    for (int i=0; i<4; i++) {
        *ptr++='a';
        *ptr++=0xc;
    }
    while(1);
}