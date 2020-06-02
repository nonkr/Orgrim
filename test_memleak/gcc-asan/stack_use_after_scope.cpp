volatile int *p = nullptr;

int main() {
    {
        int x = 0;
        p = &x;
    }
    *p = 5;
    return 0;
}
