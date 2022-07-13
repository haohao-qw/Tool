#include"buffer.hpp"
#include<stdio.h>

int main(){
	FILE* fp=NULL;
	fp=fopen("log/test.txt","w");
	fprintf(fp,"test is ok\n");
	fputs("fputs is ok\n",fp);
	Buffer buff(1000);
	const char* src="buffer used is right....\n";
	buff.append(src,50);
	buff.try_write(fp);
	fclose(fp);
	printf("可用长度:%d\n",buff.available_len());
	printf("原始数据:%s\n",buff.getdata());//写入成功
	return 0;
	
}
