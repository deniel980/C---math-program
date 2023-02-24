/*
 Testat für PDA-2022/23, HS Kaiserslautern / Prof. Bomarius

 Version: 1.7.1

 Autor: Daniel Weiner

 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include <math.h>

///////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	int n;
	double h;

	double x_start;
	double x_final;
	double x_user;
	double x1;
	double x2;

	double y_start;
	double y_final;
	double y1;
	double y2;
	double y_user;
} ip;

double *array;
double max_dbl= DBL_MAX/pow(10,295);
int max_int = INT_MAX;

//-------------------------------------------------------------------------------
int get_input_manager(ip *input); //controls the get_input function to reduce the size of the main()
int get_input(ip *input, int option, char *prompt); //gets all the input from the user
int get_line(ip *input, char *prompt, char*error);//gets the y-values, allocates the array, assigns the values, prints out the coordinates
int custom_y_expected(ip *input, char*error);//gets the custom x-input and calculates custom y-value, treats expected x-inputs
int custom_y_unexpected(ip*input, char*error, int control_x, int length_line, int get_out, int k); //treating unexpected custom x-inputs

///////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	setvbuf(stdout, NULL, _IONBF, 0);
	char *welcome = "\nWelcome to the Wizard!\n";

	ip input = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };



	printf(welcome);

	printf("\n(This program allows precise display of up to 3 decimal places for the inputs of double type.)\n");

	get_input_manager(&input);

	get_line(&input, "\nPlease, give in the y-values of your grid points: ", "\nYour input is out of range.\n");

	for (int i = 0; i <= 10; i++) {
		custom_y_expected(&input, "\nYour input is out of range.\n");
	}

	free(array);
	printf("\nThe memory is de-allocated, I hope you had fun.\n");

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int get_input_manager(ip *input) {

	do{
		get_input(input, 1, "\nPlease, give in the wished number of grid points: ");
		printf("\nThis is the test. Read n-value is: %d\n", input->n);
		get_input(input, 2, "\nPlease, give in the x-value of the first grid point: ");
		printf("\nThis is the test. Read n-value is: %f\n", input->x_start);
		get_input(input, 3, "\nPlease, give in the h (x-step-size): ");

		input->x_final = input->x_start + (input->h * (input->n-1));

		if(input->x_final>=(max_dbl-1) || input->x_final<=(-max_dbl+1)){
			printf("\nYour inputs are out of range.\n");
		}
	}while(input->x_final>=(max_dbl-1) || input->x_final<=(-max_dbl+1));


	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int get_input(ip*input, int option, char*prompt){
	int control=0;
	char*error="\nYour input is out of range.\n";

	do{
		printf(prompt);
		fflush(stdin);

		switch(option){
		case 1:
				control=scanf("%d", &input->n);
			break;
		case 2:
				control=scanf("%lf", &input->x_start);
			break;
		case 3:
				control=scanf("%lf", &input->h);
			break;
		}
		if(control!=1||input->n<0){
					printf(error);
				}
	}while(control!=1||input->n<0);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int get_line(ip *input, char *prompt, char *error) {

//----------------allocating array----------------------------
	int length_line= input->n;
	int length_array = 2 * (length_line);

	array = (double*) malloc(sizeof(double) * length_array);

	if (array == NULL) {
		printf("\nMemory not available.\n");
		free(array);
		exit(1);
	}
//----------------getting and assigning the y-values-------------

	for (int i = 0; i <= (length_line-1); i++) {
		int control = 0;
		do {
			printf("\n y-value for the point no.%d: ", (i+1));
			fflush(stdin);
			control = scanf("%lf", (array + i + length_line + 1));

			if ((control != 1) || (*(array + i + length_line + 1) > (max_dbl-1))) {
				printf(error);
			}
		} while ((control != 1) || (*(array + i + length_line + 1) > (max_dbl-1)));
	}

//----------------creating x-values and printing the list----------
	int i = 0;
	do {
		*(array + i) = input->x_start + input->h * i;
		i++;
	} while (i < length_line);

	for (int j = 0; j < input->n; j++) {
		printf("\nCoordinates for point no.%d: (%f, %f)\n", (j + 1), *(array + j), *(array + (j + (length_line + 1))));
	}

	input->y_start = *(array+length_line + 1);
	input->y_final = *(array+(2*length_line-1) + 1);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int custom_y_expected(ip *input, char*error){
	int control_x = 0;
	int length_line= input->n;

	int get_out=0;

	printf("\nPlease enter your custom x-value: ");
	fflush(stdin);
	control_x = scanf("%lf", &input->x_user);

	int k = 0;

	while(1==control_x && input->x_user < input->x_final  &&  input->x_user > input->x_start  &&  k <= (length_line - 1)){
		int stop=0;

		if (input->x_user < *(array + k + 1) && input->x_user > *(array + k) && stop!=1) {
			input->x2 = *(array + k + 1);
			input->x1 = *(array + k);
			printf("\nSuccess: x1 and x2 are determined: %lf and %lf", input->x1, input->x2);
			input->y2 = *(array + k + (length_line + 2));
			input->y1 = *(array + k + (length_line + 1));
			printf("\nSuccess: y1 and y2 are determined: %lf and %lf", input->y1, input->y2);

			input->y_user = input->y1 + ((input->x_user - input->x1) / (input->x2 - input->x1) * (input->y2 - input->y1));
			stop=1;
			get_out=1;
		}
		k++;
	}
	if(0==get_out){//treating unexpected inputs
		custom_y_unexpected(input,error,control_x,length_line,get_out,k);
	}

	printf("\nYour requested y-value is: %f", input->y_user);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int custom_y_unexpected(ip*input, char*error, int control_x, int length_line, int get_out, int k){

	if (control_x != 1) {
		printf( "\nYour input is not a digit and will be replaced by the median value.\n");
		input->x_user = input->x_start + (input->x_final - input->x_start) / 2;
		input->y_user = input->y_start + (input->y_final - input->y_start) / 2;
		get_out = 1;
	}
	if (input->x_user < input->x_start && get_out != 1) {
		printf( "\nYour input is out of range (too small) and will be replaced by the smallest possible value (x_start).\n");
		input->x_user = input->x_start;
		input->y_user = input->y_start;
		get_out = 1;
	}
	if (input->x_user > input->x_final && get_out != 1) {
		printf( "\nYour input is out of range (too big) and will be replaced by the biggest possible value (x_final).\n");
		input->x_user = input->x_final;
		input->y_user = input->y_final;
		get_out = 1;
	}

	if (get_out != 1) { //this should add the instruction for the situation where x_user is equal to an existing point
		k = 0;
		int stop = 0;
		while (k <= (length_line - 1) && stop != 1) {

			if (*(array + k) == input->x_user) {
				input->y_user = *(array + k + (length_line + 1));
				printf("\nYour input is equal to x-value of an already existing point no.%d.\n", (k + 1));
				stop = 1;
			}
			k++;
		}
		get_out = 1;
	}
	return 0;
}
