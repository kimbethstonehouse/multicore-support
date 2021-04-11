int main() {
	int nr = 0;
	asm volatile("int $0x80" :: "a"((unsigned long long int)nr));
	return 0;
}
