#include<stdio.h>
#include<stdlib.h>
typedef struct node{
	int data;
	struct node *left;
	struct node *right;

}node;
node *a[10000];
int function(int num)
{
	return num%1000;
}

node *insert(int num,node *head)
{
	if(head==NULL)
	{
		node *new=(node*)malloc(sizeof(node));
		new->data=num;
		new->left=NULL;
		new->right=NULL;
		head=new;
		return head;
	}
	else if(head->data > num)
	{
		head->left=insert(num,head->left);
	}
	else 
	{
	    	head->right=insert(num,head->right);
	}
	return head;
}
/*void delete(int num)
{
	int ins=function(num),state=0;
	node *temp=a[ins]->next;
	while(temp!=NULL)
	{
		if(temp->data==num)
		{
			state=1;
			break;
		}
		temp=temp->next;
	}
	if(state==1)
		temp=temp->next;
	
	
}*/
//int f=0;
int search(int val,node *head)
{
    int f;
    if(head==NULL)
    {
	return 0;
    }
    else if(head->data==val)
    {
	//printf("Found\n");
	//f=1;
	return 1;
    }
    else if(head->data > val)
    {
	f=search(val,head->left);	
    }
    else
    {
	f=search(val,head->right);
    }
    return f;
}
int main()
{
	int num,i,pos;
	char c,x;
	for(i=0;i<1001;i++)
	{
		a[i]=(node*)malloc(sizeof(node));
	}
	while(1)
	{
		scanf("%c ",&c);
		if(c=='I')
		{
			scanf("%d",&num);
			pos=function(num);
			if(search(num,a[pos])!=1)
			{
			 a[pos]=insert(num,a[pos]);
			}
		}
	//	else if(c=='D')
	//	{
	//		scanf("%d",&num);
	//		delete(num);
	//	}
		else if(c=='S')
		{
			scanf("%d",&num);
			int ins=function(num);
			int f=search(num,a[ins]);
			if(f==1)
			{
				printf("Found\n");
			}
			else
			{
				printf("Not Found\n");
			}
		}
		else if(c=='Q')
		{
			break;
		}
		scanf("%c",&x);
	}
	return 0;
}
