#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

float glass_w, glass_h, glass_d;
float product_w, product_h;
float no_tails_w, no_tails_h;

char * generate_str(char * str, char * ct, float arg1, float arg2, float arg3, float arg4){  
    sprintf(ct, str, arg1, arg2, arg3, arg4);
    return ct;
}

void generate_gcode(float product_w, float product_h, char * result){

    float tail_w = ((2.0 * glass_d) + ((no_tails_w - ((int)(no_tails_w / product_w) * product_w)) / 2));
    float tail_h = ((2.0 * glass_d) + ((no_tails_h - ((int)(no_tails_h / product_h) * product_h)) / 2));

    char * HEADER = "M3\nG21\n";
    char * FOOTER = "M5\nG00 X0.0000 Y0.0000\nM2";
    char * template = "G00 Z5.000000\n\
G00 X%f Y%f\n\
G01 Z-1.000000 F100.0\n\
G01 X%f Y%f Z-1.000000 F400.000000\n\
G00 Z5.000000\n";

    char temp_copy[1024];
    float gap_w, gap_h;
    gap_w = product_w;
    gap_h = glass_h - tail_h - product_h;

    int counter = 0;

    strcat(result, HEADER);
    strcat(result, generate_str(template, temp_copy, tail_w, -glass_h, tail_w, 0.0));

    while (counter < (int)(no_tails_w / product_w) - 1) {
        strcat(result, generate_str(template, temp_copy, tail_w + gap_w, -glass_h + tail_h, tail_w + gap_w, -tail_h));
        gap_w += product_w;
        counter += 1;
    }

    strcat(result, generate_str(template, temp_copy, glass_w - tail_w, -glass_h, glass_w - tail_w, 0.0));

    strcat(result, generate_str(template, temp_copy, glass_w, -glass_h + tail_h, 0, -glass_h + tail_h));

    counter = 0;

    while (counter < (int)(no_tails_h / product_h) - 1) {
        strcat(result, generate_str(template, temp_copy, glass_w - tail_w, -gap_h, tail_w, -gap_h));
        gap_h -= product_h;
        counter += 1;
    }

    strcat(result, generate_str(template, temp_copy, glass_w, -tail_h, 0.0, -tail_h));

    strcat(result, FOOTER);

}

int main(){

    int ratio_ww, ratio_hh, ratio_wh, ratio_hw;

    char result[128000];

    printf("Glass width: ");

    if(scanf("%f", &glass_w) == 0 || glass_w < 0){
        printf("Wrong argument.");
        exit(-1);
    }

    printf("Glass height: ");

    if(scanf("%f", &glass_h) == 0 || glass_h < 0){
        printf("Wrong argument.");
        exit(-1);
    }

    printf("Glass depth(4 or 8): ");

    if(scanf("%f", &glass_d) == 0 || glass_d < 0){
        printf("Wrong argument.");
        exit(-1);
    }

    glass_d = glass_d <= 4 ? 4 : 8; 

    printf("Product width: ");

    if(scanf("%f", &product_w) == 0 || product_w < 0){
        printf("Wrong argument.");
        exit(-1);
    }    

    printf("Product height: ");

    if(scanf("%f", &product_h) == 0 || product_h < 0){
        printf("Wrong argument.");
        exit(-1);
    }    

    no_tails_w = glass_w - 4.0 * glass_d;
    no_tails_h = glass_h - 4.0 * glass_d;

    if(MAX(no_tails_w, no_tails_h) >= MAX(product_w, product_h) && MIN(no_tails_w, no_tails_h) >= MIN(product_w, product_h)){
        
        ratio_ww = no_tails_w / product_w;
        ratio_hh = no_tails_h / product_h;
        ratio_wh = no_tails_w / product_h;
        ratio_hw = no_tails_h / product_w;

        if(glass_w < glass_h){
            
            if((ratio_ww * ratio_hh) > (ratio_wh * ratio_hw)){
                
                generate_gcode(product_w, product_h, result);

            } else {
                
                generate_gcode(product_h, product_w, result);

            }
        } else {

            if((ratio_ww * ratio_wh) > (ratio_wh * ratio_hw)){
                
                generate_gcode(product_w, product_h, result);

            } else {
                
                generate_gcode(product_h, product_w, result);

            }
        }
        
    } else {
        printf("Wrong arguments.");
        exit(-1);
    }

    char filename[128];

    sprintf(filename, "glass_%.1fx%.1f(%.1fx%.1f).ngc", glass_w, glass_h, product_w, product_h);

    FILE * f = fopen(filename, "w");

    fprintf(f, "%s", result);

    fclose(f);

    return 0;
}