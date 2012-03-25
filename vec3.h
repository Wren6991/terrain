class vec3
{
    public:
    double x;
    double y;
    double z;
    vec3();
    vec3(double, double, double);
    vec3 operator+(vec3);
    vec3 operator-(vec3);
    vec3 operator*(double);
    vec3 normalise();
    double length();
};
