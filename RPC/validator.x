
struct point {
    float x;
    float y;
    float z;
    int r;
    int g;
    int b;
};

program PVAL {
    version VALIDATOR {
        int VALIDATE_POINT(point) = 1;
    } = 1;
} = 0x2fffffff;