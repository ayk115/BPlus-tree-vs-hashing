#include<stdio.h>
#include<stdlib.h>
typedef struct record{
    int val;
}record;
#define order 3
typedef struct node{
    int *data;
    void **pointer;
    int size;
    struct node *parent;
    struct node *next;
    int is_leaf;
}node;

node *make_leaf();
node *make_node();
node *insert_parent(node *,node *,int,node *);
node *insert_internal_splitting(node *,node *,int,int,node*);
node *queue=NULL;
node* dequeue();
void enqueue(node *);
node *delete_entry(node *,node *,int,void *);
node * redistribute_nodes(node *, node *, node *, int,int,int); 
int cut(int);

int cut(int ord)
{
    if(ord%2==0)
    {
	return ord/2;
    }
    else
    {
	return (ord/2)+1;
    }
}

node * insert_after_splitting(node *root,int key,node *leaf,record *pointer)
{
    node *new_leaf;
    int new_key,split;
    new_leaf=make_leaf();
    int *temp=(int *)malloc((order)*sizeof(int));
    void **point=(void **)malloc((order)*sizeof(void *));
    int insertion_index = 0,i,j;
    /*here we are inserting the values of current node's values in the temporary array temp and pointers in point*/
    while(insertion_index < order - 1 && leaf->data[insertion_index] < key)
	insertion_index++;
    for (i=0,j=0; i < leaf->size; i++, j++) 
    {
	if(j==insertion_index) j++;
	temp[j]=leaf->data[i];
	point[j]=leaf->pointer[i];
    }
    /*here the value of key is inserted  at the appropriate position*/
    temp[insertion_index]=key;
    point[insertion_index]=pointer;
    /*for condition of the ceil(n+1/2) for b+tree for the current node */
    leaf->size=0;
    split=cut(order-1);
    /*filling those values into the current leaf node till split*/
    for (i = 0; i < split; i++) 
    {
	leaf->pointer[i]=point[i];
	leaf->data[i]=temp[i];
	leaf->size++;
    }
    /*filling the rest values in the newly created node */
    for (i=split,j=0;i<order;i++,j++) 
    {
	new_leaf->pointer[j] = point[i];
	new_leaf->data[j] = temp[i];
	new_leaf->size++;
    }
    free(temp);
    free(point);
    /*managing the link among the leaf nodes */
    new_leaf->pointer[order-1] = leaf->pointer[order - 1];
    leaf->pointer[order-1] = new_leaf;
    /*making rest pointers to Null*/
    for (i = leaf->size; i < order - 1; i++)
	leaf->pointer[i] = NULL;
    for (i = new_leaf->size; i < order - 1; i++)
	new_leaf->pointer[i] = NULL;

    new_leaf->parent=leaf->parent;
    new_key = new_leaf->data[0];
    /*now it is the time to insert the corresponding keys in the parent node*/
    return insert_parent(root,leaf,new_key,new_leaf);
}


node *insert_parent(node *root,node *left,int key,node *right)
{
    int left_index,i;
    node * parent;

    parent=left->parent;

    /* this is the case when we have to create the common parent for both and also for root*/
    if(parent == NULL)
    {
	node *new_root=make_node();
	new_root->data[0]=key;
	new_root->pointer[0]=left;
	new_root->pointer[1]=right;
	new_root->size++;
	new_root->parent=NULL;
	left->parent=new_root;
	right->parent=new_root;
	return new_root;
    }
    /*now if parent exists then we have to find the location of the pointers and keys */
    int l_index=0;
    while(l_index<=parent->size && parent->pointer[l_index]!=left)
    {
	l_index++;
    }
    /*this is the case when we have to just put the key into the parent node as their is vacant space in parent*/
    if(parent->size<order-1)
    {
	for(i=parent->size;i>l_index;i--)
	{
	    parent->pointer[i+1]=parent->pointer[i];
	    parent->data[i]=parent->data[i-1];
	}
	parent->pointer[l_index+1]=right;
	parent->data[l_index]=key;
	parent->size++;
	return root;
    }
    /*now parent was already filled so we have to use the splitting condition and create a new node*/
    return insert_internal_splitting(root,parent,l_index,key,right);

}

node *insert_internal_splitting(node *root,node *parent,int l_index,int key,node *right)
{
    int i,j,k,split;
    node *new_node,*child;
    int *temp;
    node **point;
    temp=(int *)malloc((order)*sizeof(int));
    point=(node **)malloc((order+1)*sizeof(node *));
    /*moving the pointers to new pointer except l_index+1 as here we have to save the keys and create corresponding pointers*/
    for(i=0,j=0;i<parent->size+1;i++,j++) 
    {
	if(j==l_index+1) j++;
	point[j] = parent->pointer[i];
    }
    /*moving the keys in above manner*/
    for(i=0,j=0;i<parent->size;i++,j++) //#####################
    {
	if (j==l_index)j++;
	temp[j]=parent->data[i];
    }
    point[l_index+1]=right;
    temp[l_index]=key;
    split=cut(order);
    new_node=make_node();
    parent->size=0;
    for (i = 0; i < split - 1; i++) 
    {
	parent->pointer[i]=point[i];
	parent->data[i]=temp[i];
	parent->size++;
    }
    parent->pointer[i]=point[i];
    k=temp[split - 1];
    for (++i,j=0; i<order; i++, j++) 
    {
	new_node->pointer[j]=point[i];
	new_node->data[j]=temp[i];
	new_node->size++;
    }
    new_node->pointer[j]=point[i];
    free(temp);
    free(point);
    new_node->parent=parent->parent;
    for (i = 0; i <= new_node->size; i++) 
    {
	child = new_node->pointer[i];
	child->parent = new_node;
    }
    return insert_parent(root,parent,k,new_node);
}

node *find_leaf(node *root,int keys)
{
    int i;
    node *c=root;
    if(c==NULL)
    {
	return c;
    }
    while(c->is_leaf==0)//searching for the data in the tree
    {
	i=0;
	while(i<c->size)
	{
	    if(keys >= c->data[i]) i++;
	    else break;
	}
	c=(node *)c->pointer[i];
    }
    return c;
}

node *make_node()
{
    node *new;
    new=(node *)malloc(sizeof(node));
    new->data=(int *)malloc((order-1)*sizeof(int));//malloc for the blocks of data
    new->pointer=(void **)malloc((order)*sizeof(void *));//malloc for the blocks of pointer 
    new->is_leaf=0;
    new->size=0;
    new->parent=NULL;
    new->next=NULL;
    return new;
}

node *make_leaf()
{
    //creating the leaf node with leaf flag=1
    node *temp=make_node();
    temp->is_leaf=1;
    return temp;
}

node *insert(node *root,int keys,int p)
{
    record *new_pointer=(record *)malloc(sizeof(record));
    new_pointer->val=p;
    node *leaf;
    /*if tree is empty*/
    if(root==NULL)
    {
	node *new=make_leaf();
	new->data[0]=keys;
	new->pointer[0]=new_pointer;
	new->pointer[order-1]=NULL;
	new->parent=NULL;
	new->is_leaf=1;
	new->size=1;
	return new;
    }
    leaf=find_leaf(root,keys);
    if(leaf->size<order-1)
    {
	int i, insertion_point;
	insertion_point = 0;
	while(insertion_point<leaf->size && leaf->data[insertion_point] < keys)
	{
	    insertion_point++;
	}
	for(i=leaf->size;i>insertion_point; i--) 
	{
	    leaf->data[i] = leaf->data[i - 1];
	    leaf->pointer[i] = leaf->pointer[i - 1];
	}
	leaf->data[insertion_point]=keys;
	leaf->pointer[insertion_point]=new_pointer;
	leaf->size++;
	return root;
    }
    return insert_after_splitting(root,keys,leaf,new_pointer);
}
//////////////////////////////////////////////////////////Print ///////////////////////////////////////////////////////////////////////////
void print( node * root ) 
{
    int i;
    node * c = root;
    if (root == NULL) 
    {
	//printf("\n");
	return;
    }
    while (c->is_leaf==0)
	c = c->pointer[0];
    while(1)
    {
	if(c->pointer[order-1]==NULL)
	{
		for (i = 0; i < c->size; i++) 
		{
		    if(i==c->size-1)
		    {
		    	printf("%d", c->data[i]);
		    }
		    else
		    {
		    	printf("%d ", c->data[i]);
		    }
		}

	}
	else
	{
		for (i = 0; i < c->size; i++) 
		{
		    	printf("%d ", c->data[i]);
		}
	}
	if (c->pointer[order - 1] != NULL) 
	{
	    c = c->pointer[order - 1];
	}
	else
	    break;
    }
    //printf("\n");
}
/////////////////////////////////////////////////////////////////deletion begins//////////////////////////////////////////////////////////
node * change_root(node * root) 
{
    node * new_root;
    /* Case: nonempty root. Key and pointer have already been deleted,so nothing to be done.*/
    if (root->size > 0)
	return root;
    /* Case: empty root.If it has a child, promote the first (only) child as the new root.*/
    if (root->is_leaf==0) 
    {
	new_root = root->pointer[0];
	new_root->parent = NULL;
    }
    // If it is a leaf (has no children),then the whole tree is empty.
    else
	new_root = NULL;
    free(root->data);
    free(root->pointer);
    free(root);
    return new_root;
}
int get_neighbor_index(node * n) 
{
    int i;
    /* Return the index of the key to the left of the pointer in the parent pointing to n. If n is the leftmost child return -1.*/
    for (i=0;i<=n->parent->size; i++)
    {
	if (n->parent->pointer[i] == n)
	    return i - 1;
    }
}
node * coalesce_nodes(node * root, node * n, node * neighbor,int neighbor_index,int k_parent) 
{
    int i, j, neighbor_insertion_index, n_start, n_end, new_k_prime;
    node * tmp;
    int split;
    /* Swap neighbor with node if node is on the extreme left and neighbor is to its right.*/
    if (neighbor_index == -1) 
    {
	tmp = n;
	n = neighbor;
	neighbor = tmp;
    }
    /* Starting point in the neighbor for copying keys and pointers from n.*/
    neighbor_insertion_index = neighbor->size;
    /* if the insertion of k_parent would cause the resulting single coalesced node to exceed the limit order - 1.*/
    split = 0;
    /* Append k_parent and the following pointer.If there is room in the neighbor, append all pointers and keys from the neighbor.
       Otherwise, append only cut(order) - 2 keys and cut(order) - 1 pointers.*/
    if (n->is_leaf==0) 
    {
	/* Append k_parent.*/
	neighbor->data[neighbor_insertion_index]=k_parent;
	neighbor->size++;
	/* there is room for all of n's keys and pointers in the neighbor after appending k_parent.*/
	n_end = n->size;
	/*k cannot fit with all the other keys and pointers into one coalesced node.*/
	n_start = 0; 
	if (n->size + neighbor->size >= order) 
	{
	    split =1;
	    n_end = cut(order) - 2;
	}
	for (i = neighbor_insertion_index + 1, j = 0; j <n_end; i++, j++) 
	{
	    neighbor->data[i] = n->data[j];
	    neighbor->pointer[i] = n->pointer[j];
	    neighbor->size++;
	    n->size--;
	    n_start++;
	}
	/* The number of pointers is always one more than the number of keys.*/
	neighbor->pointer[i] = n->pointer[j];
	/* If the nodes are still split, remove the first key from n.*/
	if (split==1) 
	{
	    new_k_prime = n->data[n_start];
	    for (i = 0, j=n_start + 1; i < n->size; i++, j++) {
		n->data[i] = n->data[j];
		n->pointer[i] = n->pointer[j];
	    }
	    n->pointer[i] = n->pointer[j];
	    n->size--;
	}
	/* All children must now point up to the same parent.*/
	for (i = 0; i < neighbor->size+ 1; i++) {
	    tmp = (node *)neighbor->pointer[i];
	    tmp->parent = neighbor;
	}
    }
    /* In a leaf, append the keys and pointers of n to the neighbor.Set the neighbor's last pointer to point to what had been n's right neighbor.*/
    else {
	for (i = neighbor_insertion_index, j = 0; j < n->size; i++, j++) 
	{
	    neighbor->data[i] = n->data[j];
	    neighbor->pointer[i] = n->pointer[j];
	    neighbor->size++;
	}
	neighbor->pointer[order - 1] = n->pointer[order - 1];
    }	
    if (split==0) 
    {
	root=delete_entry(root,n->parent,k_parent,n);
	free(n->data);
	free(n->pointer);
	free(n); 
    }
    else
	for (i = 0; i < n->parent->size; i++)
	    if (n->parent->pointer[i + 1] == n) 
	    {
		n->parent->data[i]=new_k_prime;
		break;
	    }

    return root;

}



node * delete_entry(node * root,node * n,int key,void * pointer) 
{
    int min_size,capacity,neighbor_index;
    node * neighbor;
    int k_parent_ind,k_parent;
    int i=0,point;
    // Remove key and pointer from node.
    while(n->data[i]!=key)//searching for the key in the node
    {
	i++;  
    }
    for(++i;i<n->size;i++)//shifting the keys in the node
    {
	n->data[i-1]=n->data[i];
    }
    point=n->is_leaf?n->size:n->size+1;//for detecting the no. of pointers in the present node
    i=0;
    while(n->pointer[i]!=pointer)//searching for the pointer in the node
    {
	i++;
    }
    for(++i;i<point;i++)//shifting the pointer till the last pointer 
    {
	n->pointer[i-1]=n->pointer[i];
    }
    n->size--;
    if(n->is_leaf)
    {
	for (i = n->size; i < order - 1; i++)//it is till order-1 as the last pointer is pointing to the next node 
	    n->pointer[i] = NULL;
    }
    else
    {	    
	for (i = n->size+ 1; i < order; i++)
	    n->pointer[i] = NULL;
    }
    /* Case:  deletion from the root.  */
    if (n == root) 
	return change_root(root);
    /* Case:  deletion from a node below the root.*/
    min_size=n->is_leaf ? cut(order-1) : cut(order) - 1;//minimum allowable size for the node ceil(order/2) or floor(order/2)
    /* Case:  node stays at or above minimum.*/
    if (n->size>=min_size)
	return root;
    /* Find the appropriate neighbor node with which to coalesce. 
     * Also find the key (k_prime) in the parent between the pointer to node n and the pointer to the neighbor.*/

    neighbor_index=get_neighbor_index(n);
    k_parent_ind=neighbor_index==-1?0:neighbor_index;//###################
    k_parent=n->parent->data[k_parent_ind];
    neighbor=neighbor_index==-1?n->parent->pointer[1] : 
	n->parent->pointer[neighbor_index];
    capacity = n->is_leaf?order:order - 1;

    /* Coalescence. */
    if(neighbor->size+n->size < capacity)
	return coalesce_nodes(root,n,neighbor,neighbor_index,k_parent);
    /* Redistribution. */
    else
	return redistribute_nodes(root,n,neighbor,neighbor_index,k_parent_ind,k_parent);
}

node * redistribute_nodes(node * root, node * n, node * neighbor, int neighbor_index,int k_parent_ind, int k_parent) 
{  
    int i;
    node * tmp;
    /* Case: n has a neighbor to the left. Pull the neighbor's last key-pointer pair over from the neighbor's right end to n's left end.*/
    if (neighbor_index!= -1) 
    {
	if (n->is_leaf==0)
	    n->pointer[n->size+ 1] = n->pointer[n->size];
	for (i = n->size; i > 0; i--) 
	{
	    n->data[i] = n->data[i - 1];
	    n->pointer[i] = n->pointer[i - 1];
	}
	if (n->is_leaf==0) 
	{
	    n->pointer[0] = neighbor->pointer[neighbor->size];
	    tmp = (node *)n->pointer[0];
	    tmp->parent = n;
	    neighbor->pointer[neighbor->size] = NULL;
	    n->data[0] = k_parent;
	    n->parent->data[k_parent_ind] = neighbor->data[neighbor->size - 1];
	}
	else 
	{
	    n->pointer[0] = neighbor->pointer[neighbor->size - 1];
	    neighbor->pointer[neighbor->size - 1] = NULL;
	    n->data[0] = neighbor->data[neighbor->size - 1];
	    n->parent->data[k_parent_ind] = n->data[0];
	}
    }
    /*Take a key-pointer pair from the neighbor to the right.Move the neighbor's leftmost key-pointer pair to n's rightmost position.*/
    else 
    {  
	if (n->is_leaf==1) 
	{
	    n->data[n->size] = neighbor->data[0];
	    n->pointer[n->size] = neighbor->pointer[0];
	    n->parent->data[k_parent_ind] = neighbor->data[1];
	}
	else 
	{
	    n->data[n->size] = k_parent;
	    n->pointer[n->size + 1] = neighbor->pointer[0];
	    tmp = (node *)n->pointer[n->size + 1];
	    tmp->parent = n;
	    n->parent->data[k_parent_ind] = neighbor->data[0];
	}
	for (i = 0; i < neighbor->size; i++) 
	{
	    neighbor->data[i] = neighbor->data[i + 1];
	    neighbor->pointer[i] = neighbor->pointer[i + 1];
	}
	if (n->is_leaf==0)
	    neighbor->pointer[i] = neighbor->pointer[i + 1];
    }
    /* n now has one more key and one more pointer;the neighbor has one fewer of each.*/
    n->size++;
    neighbor->size--;
    return root;
}

node * delete(node * root, int key) 
{
    int i;
    node * k_leaf,*c;
    record *p;
    c=find_leaf(root,key);
    if(c==NULL)
    {
	p=NULL;
    }
    for(i=0;i<c->size;i++)
    {
	if(c->data[i]==key)
	{
	    break;
	}
    }
    if(i==c->size)
    {
	p=NULL;
    }
    else
    {
	p=(record *)c->pointer[i];
    }
    k_leaf=find_leaf(root, key);
    if (p!= NULL && k_leaf!= NULL) {
	root = delete_entry(root,k_leaf,key,p);
	free(p);
    }
    return root;
}

int main()
{
    char c;
    node *root=NULL,*temp,*f;
    int val,i;
    while(1)
    {
	scanf("%c",&c);
	getchar();
	if(c=='I')
	{
	    scanf("%d",&val);
	    root=insert(root,val,val);
	    getchar();
	}
	if(c=='S')
	{
	    scanf("%d",&val);
	    getchar();
	    f=find_leaf(root,val);
	    if(f==NULL)
	    {
		printf("Not Found\n");
	    }
	    else
	    {
	    	for(i=0;i<f->size;i++)
	    	{
			if(f->data[i]==val)
			{
			    break;
			}
		    }
		    if(i==f->size)
		    {
			printf("Not Found\n");
		    }
		    else
		    {
			printf("Found\n");
		    }
	    }
	}
	if(c=='P')
	{
	    print(root);
	    printf("\n");
	}
	if(c=='D')
	{
	    scanf("%d",&val);
	    root=delete(root,val);
	    getchar();
	}
	if(c=='Q')
	{
	    break;
	}
    }
}
