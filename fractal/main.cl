#pragma OPENCL EXTENSION cl_khr_fp64 : enable

double2 cpow(double2 c, int num){
    double x = c.x, y = c.y;
    if (num == 1){
        return c;
    } else if ( num == 2){
        return (double2)(x * x - y * y, 2.0 * x * y);
    } else if ( num == 3){
        return (double2)(x * x * x - 3.0 * x * y * y, 3.0 * x * x * y - y * y * y);
    } else if ( num == 4){
        return (double2)(x * x * x * x - 6.0 * x * x * y * y + y * y * y * y, 4.0 * x * x * x * y - 4.0 * x * y * y * y);
    } else if ( num == 5){
        return (double2)(x*x*x*x*x-10.0*x*x*x*y*y+5.0*x*y*y*y*y,5.0*x*x*x*x*y-10.0*x*x*y*y*y+y*y*y*y*y);
    }
    return (double2)(0,0);
}

double2 cpow2(double2 c){
   return (double2)((c.x * c.x) - (c.y * c.y), 2.0 * c.x * c.y);
}

double2 cadd(double2 c1, double2 c2){
   return (double2)(c1.x+c2.x,c1.y+c2.y);
}

double2 csub(double2 c1, double2 c2){
   return (double2)(c1.x-c2.x,c1.y-c2.y);
}

double cabs(double2 c){
   return c.x*c.x + c.y* c.y;
}

double2 cmulti(double2 c1, double2 c2){
   return (double2)((c1.x*c2.x-c1.y*c2.y),(2.0 * c1.y*c2.x));
}

double2 cdiv(double2 c1, double2 c2){
   return (double2)((c1.x*c2.x-c1.y*c2.y),(c1.x*c2.y+c1.y*c2.x));
}

double2 cmultiNum(double2 c, double num){
   return (double2)((c.x*num),(c.y*num));
}

__kernel void mandelbrot(__global int* w_h_iter,
    __global double* hx_hy_saw_sah,
    __global double* cx_cy,
    __global int* hybrid,
    __global int* matrix) {
    int id = get_global_id(0);
    int i;
    int x = id / w_h_iter[1];
    int y = id % w_h_iter[1];
    double x_ = (hx_hy_saw_sah[0] - hx_hy_saw_sah[2] / 2) + x * (hx_hy_saw_sah[2] / (double)w_h_iter[0]);
    double y_ = (hx_hy_saw_sah[1] - hx_hy_saw_sah[3] / 2) + y * (hx_hy_saw_sah[3] / (double)w_h_iter[1]);
    double2 xy = (double2)(x_,y_);
    double2 z = (double2)(0.,0.);
    int it = 0;
    for (i = 0; i < w_h_iter[2]; i++) {
        z = cpow2(z);
        z = cadd(z,xy);
        if (cabs(z) >= 4) {
            it = i;
            break;
        }
    }
    matrix[y * w_h_iter[0] + x] = it;
}

__kernel void julia(__global int* w_h_iter,
    __global double* hx_hy_saw_sah,
    __global double* cx_cy,
    __global int* hybrid,
    __global int* matrix) {
    int id = get_global_id(0);
    int i;
    int x = id / w_h_iter[1];
    int y = id % w_h_iter[1];
    double2 c_xy =(double2)(cx_cy[0],cx_cy[1]);
    double x_ = (hx_hy_saw_sah[0] - hx_hy_saw_sah[2] / 2) + x * (hx_hy_saw_sah[2] / (double)w_h_iter[0]);
    double y_ = (hx_hy_saw_sah[1] - hx_hy_saw_sah[3] / 2) + y * (hx_hy_saw_sah[3] / (double)w_h_iter[1]);
    double2 xy = (double2)(x_,y_);
    int it = 0;
    for (i = 0; i < w_h_iter[2]; i++) {
        xy = cpow2(xy);
        xy = cadd(xy, c_xy);
        if (cabs(xy) >= 4) {
            it = i;
			break;
        }
    }
    matrix[y * w_h_iter[0] + x] = it;
}

__kernel void burning_ship(__global int* w_h_iter,
    __global double* hx_hy_saw_sah,
    __global double* cx_cy,
    __global int* hybrid,
    __global int* matrix) {
    int id = get_global_id(0);
    int i;
    int x = id / w_h_iter[1];
    int y = id % w_h_iter[1];
    double x_ = (hx_hy_saw_sah[0] - hx_hy_saw_sah[2] / 2) + x * (hx_hy_saw_sah[2] / (double)w_h_iter[0]);
    double y_ = (hx_hy_saw_sah[1] - hx_hy_saw_sah[3] / 2) + y * (hx_hy_saw_sah[3] / (double)w_h_iter[1]);
    double2 xy = (double2)(x_,y_);
    double2 z = (double2)(0.,0.);
    // double z1 = 0., z2 = 0., temp;
    int it = 0;
    for (i = 0; i < w_h_iter[2]; i++) {
        z = cpow2(z);
        if (z.y < 0){z.y *=-1;}
        z = cadd(z,xy);
        if (cabs(z) >= 4) {
            it = i;
			break;
        }
    }
    matrix[y * w_h_iter[0] + x] = it;
}

__kernel void hybrid_m_b(__global int* w_h_iter,
    __global double* hx_hy_saw_sah,
    __global double* cx_cy,
    __global int* additionally,
    __global int* matrix) {
    int id = get_global_id(0);
    int i;
    int x = id / w_h_iter[1];
    int y = id % w_h_iter[1];
    double x_ = (hx_hy_saw_sah[0] - hx_hy_saw_sah[2] / 2) + x * (hx_hy_saw_sah[2] / (double)w_h_iter[0]);
    double y_ = (hx_hy_saw_sah[1] - hx_hy_saw_sah[3] / 2) + y * (hx_hy_saw_sah[3] / (double)w_h_iter[1]);
    double2 xy = (double2)(x_,y_);
    double2 z = (double2)(0.,0.);
    int it = 0;
    int iter_m = w_h_iter[2] - w_h_iter[2]/5;
    // int iter_b = w_h_iter[2]/9;
    for (i = 0; i < w_h_iter[2]; i++) {
        z = cpow2(z); 
        if (z.y < 0 && (i % 10 > additionally[0])){z.y *=-1;}
        z = cadd(z,xy);
        if (cabs(z) >= 4) {
            it = i;
			break;
        }
    }
    matrix[y * w_h_iter[0] + x] = it;
}

__kernel void tricorn(__global int* w_h_iter,
    __global double* hx_hy_saw_sah,
    __global double* cx_cy,
    __global int* additionally,
    __global int* matrix) {
    int id = get_global_id(0);
    int i, j;
    int x = id / w_h_iter[1];
    int y = id % w_h_iter[1];
    double x_ = (hx_hy_saw_sah[0] - hx_hy_saw_sah[2] / 2) + x * (hx_hy_saw_sah[2] / (double)w_h_iter[0]);
    double y_ = (hx_hy_saw_sah[1] - hx_hy_saw_sah[3] / 2) + y * (hx_hy_saw_sah[3] / (double)w_h_iter[1]);
    double2 xy = (double2)(x_,y_);
    double2 z = (double2)(0.,0.);
    // double z1 = 0., z2 = 0., temp;
    int it = 0;
    for (i = 0; i < w_h_iter[2]; i++) {
        // z = cpow2(z);
        z = cpow(z, additionally[0]);
        // if (z.y > 0){z.y *=-1;}
        z.y *= -1;
        // if (z.x < 0){z.x *=-1;}
        z = cadd(z,xy);
        if (cabs(z) >= 4) {
            it = i;
			break;
        }
    }
    matrix[y * w_h_iter[0] + x] = it;
}