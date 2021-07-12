/**************** start of file "CommonPostProcess.fx" ***********************/

static const float BlurWeights[13] =
{
    0.002216,
    0.008764,
    0.026995,
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216,
};
//
////#define BLOOMCURVE_METHOD_1
////#define BLOOMCURVE_METHOD_2
//#define BLOOMCURVE_METHOD_3
//
//float GetBloomCurve(float x, float threshold)
//{
//    float result = x;
//    x *= 2.0f;
//
//#ifdef BLOOMCURVE_METHOD_1
//    result = x * 0.05 + max(0, x - threshold) * 0.5; // default threshold = 1.26
//#endif
//
//#ifdef BLOOMCURVE_METHOD_2
//    result = x * x / 3.2;
//#endif
//
//#ifdef BLOOMCURVE_METHOD_3
//    result = max(0, x - threshold); // default threshold = 1.0
//    result *= result;
//#endif 
//
//    return result * 0.5f;
//}
