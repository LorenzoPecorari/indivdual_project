// Author: Lorenzo Pecorari - Academic ID: 1885161
// code made only for academical purposes

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

void dir_generator(char* dir_name, char* path){
	printf("Generation of new directory... ");
	
	if(!dir_name || !path){
		printf("ERROR!\n");
		path = 0;
		return;
	}
	
	for(int i = 0; i < 128; i++)
		*(path + i) = 0;
		
	strcpy(path, "./");
	strcat(path, dir_name);
	
	struct stat s = {0};
	if(stat(path, &s) == -1)
		mkdir(path, 0700);
	
	printf("done!\n");
	return;
	
}

void subdirectory_gen(char* path){
	printf("Generation of subdirectory for main... ");
	
	if(!path){
		printf("ERROR!\n");
		path = 0;
		return;
	}
	
	strcat(path, "/main/");
	
	struct stat s = {0};
	if(stat(path, &s) == -1)
		mkdir(path, 0700);
	
	printf("done!\n");
	return;
}

void first_make_gen(char* buf, char* proj_name){
	printf("Generation and filling of CMakeLists.txt... ");
	
	if(!buf || !proj_name){
		printf("ERROR!\n"),
		buf = 0;
		return;
	}
	
	FILE* ptr = fopen(buf, "w+");
	if(!ptr){
		printf("ERROR!\n");
		buf = 0;
		return;
	}
	
	fprintf(ptr, "cmake_minimum_required(VERSION 3.16)\n\n");
	fprintf(ptr, "include($ENV{IDF_PATH}/tools/cmake/project.cmake)\n");
	fprintf(ptr, "project(");
	fprintf(ptr, proj_name);
	fprintf(ptr, ")");
	
	fclose(ptr);
	
	printf("done!\n");
	return;
}

void second_make_gen(char* buf){
	printf("Generation and filling of /main/CMakeLists.txt... ");
	
	if(!buf){
		printf("ERROR 1!\n"),
		buf = 0;
		return;
	}
	
	FILE* ptr = fopen(buf, "w+");
	if(!ptr){
		printf("ERROR 2!\n");
		buf = 0;
		return;
	}
	
	fprintf(ptr, "idf_component_register(SRCS \"main.c\" INCLUDE_DIRS \".\")");
	fclose(ptr);
	
	printf("done!\n");
	return;
	
}

int main(int argc, char** argv){

	char* path = (char*) malloc(sizeof(char) * 128);
	dir_generator(argv[1], path);
	
	if(!path){
		free(path);
		return 0;
	}
	
	char* buf = (char*) malloc(sizeof(char) * 256);
	strcpy(buf, path);
	strcat(buf, "/CMakeLists.txt");
	
	first_make_gen(buf, argv[1]);
	
	if(!buf){
		free(path);
		free(buf);
		return 0;	
	}
	
	subdirectory_gen(path);
	
	if(!path){
		free(path);
		return 0;
	}
	
	strcpy(buf, path);
	strcat(buf, "CMakeLists.txt");
		
	second_make_gen(buf);
	
	free(path);
	free(buf);
	return 0;

}
