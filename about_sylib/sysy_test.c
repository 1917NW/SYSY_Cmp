int a;
int func(int p){
	p = p - 1;
	return p;
}
int main(){
	int b;
	a = 11;
	b = func(a);
	putint(b);
	return b;
}
