//
//  ColorHandler.cpp
//  
//
//  Created by Isaac Roberts on 12/17/21.
//

// Created by Isaac Roberts


#include "ColorHandler.h"

void ColorHandler::main()
{
//	printf("value = %f\n",RGB2LAB(100,200,255, 100,200,250) );
	
	float l,a,b;
	rgb2lab(0,255,255, l, a, b);
	std::cout<<"l,a,b="<<l<<", "<<a<<", "<<b<<std::endl;
	
	
	Colour col = lab2col(l,a,b);
	std::cout<<"r,g,b="<<(int)col.getRed()<<", "<<(int)col.getGreen()<<", "<<(int)col.getBlue()<<std::endl;
}

void ColorHandler::rgb2lab( int R, int G, int B, float & l_s, float &a_s, float &b_s )
{
	float var_R = R/255.0;
	float var_G = G/255.0;
	float var_B = B/255.0;


	if ( var_R > 0.04045 ) var_R = pow( (( var_R + 0.055 ) / 1.055 ), 2.4 );
	else                   var_R = var_R / 12.92;
	if ( var_G > 0.04045 ) var_G = pow( ( ( var_G + 0.055 ) / 1.055 ), 2.4);
	else                   var_G = var_G / 12.92;
	if ( var_B > 0.04045 ) var_B = pow( ( ( var_B + 0.055 ) / 1.055 ), 2.4);
	else                   var_B = var_B / 12.92;

	var_R = var_R * 100.;
	var_G = var_G * 100.;
	var_B = var_B * 100.;

	//Observer. = 2°, Illuminant = D65
	float X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
	float Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
	float Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;


	float var_X = X / 95.047 ;         //ref_X =  95.047   Observer= 2°, Illuminant= D65
	float var_Y = Y / 100.000;          //ref_Y = 100.000
	float var_Z = Z / 108.883;          //ref_Z = 108.883

	if ( var_X > 0.008856 ) var_X = pow(var_X , ( 1./3. ) );
	else                    var_X = ( 7.787 * var_X ) + ( 16. / 116. );
	if ( var_Y > 0.008856 ) var_Y = pow(var_Y , ( 1./3. ));
	else                    var_Y = ( 7.787 * var_Y ) + ( 16. / 116. );
	if ( var_Z > 0.008856 ) var_Z = pow(var_Z , ( 1./3. ));
	else                    var_Z = ( 7.787 * var_Z ) + ( 16. / 116. );

	l_s = ( 116. * var_Y ) - 16.;
	a_s = 500. * ( var_X - var_Y );
	b_s = 200. * ( var_Y - var_Z );


}

//http://www.easyrgb.com/index.php?X=MATH&H=01#text1
void ColorHandler::lab2rgb( float l_s, float a_s, float b_s, float& R, float& G, float& B )
{
	float var_Y = ( l_s + 16. ) / 116.;
	float var_X = a_s / 500. + var_Y;
	float var_Z = var_Y - b_s / 200.;

	if ( pow(var_Y,3) > 0.008856 ) var_Y = pow(var_Y,3);
	else                      var_Y = ( var_Y - 16. / 116. ) / 7.787;
	if ( pow(var_X,3) > 0.008856 ) var_X = pow(var_X,3);
	else                      var_X = ( var_X - 16. / 116. ) / 7.787;
	if ( pow(var_Z,3) > 0.008856 ) var_Z = pow(var_Z,3);
	else                      var_Z = ( var_Z - 16. / 116. ) / 7.787;

	float X = 95.047 * var_X ;    //ref_X =  95.047     Observer= 2°, Illuminant= D65
	float Y = 100.000 * var_Y  ;   //ref_Y = 100.000
	float Z = 108.883 * var_Z ;    //ref_Z = 108.883


	var_X = X / 100. ;       //X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
	var_Y = Y / 100. ;       //Y from 0 to 100.000
	var_Z = Z / 100. ;      //Z from 0 to 108.883

	float var_R = var_X *  3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
	float var_G = var_X * -0.9689 + var_Y *  1.8758 + var_Z *  0.0415;
	float var_B = var_X *  0.0557 + var_Y * -0.2040 + var_Z *  1.0570;

	if ( var_R > 0.0031308 ) var_R = 1.055 * pow(var_R , ( 1 / 2.4 ))  - 0.055;
	else                     var_R = 12.92 * var_R;
	if ( var_G > 0.0031308 ) var_G = 1.055 * pow(var_G , ( 1 / 2.4 ) )  - 0.055;
	else                     var_G = 12.92 * var_G;
	if ( var_B > 0.0031308 ) var_B = 1.055 * pow( var_B , ( 1 / 2.4 ) ) - 0.055;
	else                     var_B = 12.92 * var_B;

	R = var_R * 255.;
	G = var_G * 255.;
	B = var_B * 255.;

}
void ColorHandler::lab2rgb( float l_s, float a_s, float b_s, int& R, int& G, int& B )
{
	float rr,gg,bb;
	lab2rgb(l_s, a_s, b_s, rr,gg,bb);
	R = jlimit(0, 255, (int)rr);
	G = jlimit(0, 255, (int)gg);
	B = jlimit(0, 255, (int)bb);
}
juce::Colour ColorHandler::lab2col(float l, float a, float B)
{
	float r, g, b;
	lab2rgb(l,a,B, r,g,b);
	return juce::Colour((int)r, (int)g, (int)b);
}



juce::ColourGradient ColorHandler::labGradient(int r1, int g1, int b1, int r2, int g2, int b2, float x1, float y1, float x2, float y2)
{
	float l1,a1,c1;
	float l2,a2,c2;
	rgb2lab(r1,g1,b1, l1, a1, c1);
	rgb2lab(r2,g2,b2, l2, a2, c2);
	
	juce::ColourGradient grad;
	grad.point1 = juce::Point<float>(x1, y1);
	grad.point2 = juce::Point<float>(x2, y2);
	
	//TODO: Set nsteps proportional to distance between (x1,y1), (x2,y2)
	int nSteps = 10;
	
	float dl = (l2-l1)/nSteps;
	float da = (a2-a1)/nSteps;
	float dc = (c2-c1)/nSteps;
	
//	float dx = (x2-x1)/nSteps;
//	float dy = (y2-y1)/nSteps;
	
	for (int n=0; n < nSteps; ++n)
	{
		float ln = l1 + n*dl;
		float an = a1 + n*da;
		float cn = c1 + n*dc;
		
		grad.addColour(float(n)/nSteps, lab2col(ln, an, cn));
	}
	return grad;
}
//static juce::ColourGradient addLabToGradient(juce::ColourGradient grad, int r2, int g2, int b2, float x2, float y2)
//{
//	float l2,a2,c2;
//	rgb2lab(r2,g2,b2, l2, a2, c2);
//}
