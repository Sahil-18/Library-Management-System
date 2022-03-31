/* My name is Sahil Purohit
My Enrollment Number is BT19CSE088
Hence I am solving problem for R3 batch i.e. Library System*/

#include<stdio.h>
#include<stdlib.h>
#define Name_Len 100
#define Book_Name_Len 100
#define Sub_Len 40
#define Dep_Len 50
#define Stat_Len 15
#define Max_Rack 4					// Maximum number of racks in a hall
#define Max_Row 5					// Maximum number of rows in one rack
#define Max_Books 10

// 4-5 order B-Tree parameters
#define Max 4
#define Min 2

// 2-3 order B-Tree parameters
#define MAX 2
#define MIN 1
typedef enum {False, True} Boolean;
typedef enum {Non_Issued,Issued} Issue_status;

static int No_of_books = 0;
static int No_of_profs = 0;
static int No_of_studs = 0;

int string_cmp(char s1[], char s2[])
{
	int i;
	i=0;
	while((s1[i]==s2[i])&&(s1[i]!='\0'))
	{
		i++;
	}
	return (s1[i]-s2[i]);
}

void string_copy(char s1[], char s2[])
{
	int i=0;
	while(s2[i]!='\0')
	{
		s1[i]=s2[i];
		i++;
	}
	s1[i]='\0';
}

// Question No. 1

// Structure defining the location of book in hall in terms of hall number, rack number and row number

typedef struct Loc_tag
{
	int hall_no,rack_no,row_no,book_no;
}Location;

// Structure defining book information
typedef struct Book_Info_node
{
	char Book_name[Book_Name_Len];
	char Subject[Sub_Len];
	char Author_name[Name_Len];
	int accession_no;
	int print_year;
	int status;
	Location Loc;
}Book_Info;

// Structure defining the information about book issued
typedef struct Issue_tag
{
	int I_dd,I_mm,I_yy;				// Date of issue/renewal
	int R_dd,R_mm,R_yy;				// Date of return
	int no_renew;					// No of times book renewed, a book can be renewed maximum upto 3 times
	int accession_no;
	struct Issue_tag* next;
}Issue;

typedef struct Member_tag
{
	char Name[Name_Len];
	char Dept[Dep_Len];
	char status[Stat_Len];			// Either Proffesor or Student
	int member_id;					
	int borrowed_no;				// Students can borrow max 2 books and Proffesors can borrow max 4 books at a time
	int fine_no;					// Stores the number of times member has paid of paying fine
	float Fine;						// Fine if book returned late or 0;
	Issue* iptr;
}Member;

typedef struct Book_Tree_node
{
	int count;
	Book_Info *Book[Max+1];					/* We will use this array from index 1 to Max, but I have taken it of length of Max+1 since this 
											will simplify further functions */
	struct Book_Tree_node *link[Max+1];	
}Book_Tree;

typedef struct Member_Tree_node
{
	int count;
	Member *member[Max+1];
	struct Member_Tree_node *link[Max+1];	
}Member_Tree;

typedef struct Library_tag
{
	Book_Tree* bptr;
	Member_Tree* Student;
	Member_Tree* Faculty;
}Library;

// Initialize Library Structure
void Initialize_DB(Library* lptr)
{
	lptr->bptr=NULL;
	lptr->Faculty=NULL;
	lptr->Student=NULL;
}

void deletebooktree(Book_Tree** root)
{
	int i;
	if(!(*root))
	{
		for(i=0;i<(*root)->count;i++)
		{
			deletebooktree(&(*root)->link[i]);
			free((*root)->Book[i+1]);
			(*root)->Book[i+1]=NULL;
		}
		deletebooktree(&((*root)->link[i]));
		free((*root));
		*root=NULL;
	}
}

void deletemembertree(Member_Tree** root)
{
	int i;
	if(!(*root))
	{
		for(i=0;i<(*root)->count;i++)
		{
			deletemembertree(&(*root)->link[i]);
			free((*root)->member[i+1]);
			(*root)->member[i+1]=NULL;
		}
		deletemembertree(&((*root)->link[i]));
		free((*root));
		*root=NULL;
	}
}

// Question No. 2
// Creating a Book node
Book_Info* create_book()
{
	Book_Info* bptr=(Book_Info*)malloc(sizeof(Book_Info));
	printf("Enter Name of Book, Subject, Author of Book, print year");
	gets(bptr->Book_name);
	gets(bptr->Subject);
	gets(bptr->Author_name);
	scanf("%s",&bptr->print_year);
	No_of_books++;
	bptr->accession_no=No_of_books;
	bptr->status=0;
	(bptr->Loc).book_no=No_of_books%Max_Books + 1;
	(bptr->Loc).rack_no=(No_of_books/Max_Books)%Max_Rack + 1;
	(bptr->Loc).row_no=((No_of_books/Max_Books)/Max_Rack)%Max_Row + 1;
	(bptr->Loc).rack_no=((No_of_books/Max_Books)/Max_Rack)/Max_Row + 1;
	return bptr;
}

// Functions to insert a book node created in above function in the Tree of Books

Book_Tree* createbookroot(Book_Info* bptr, Book_Tree* root, Book_Tree* child)
{
	Book_Tree* newnode;
	newnode = (Book_Tree*)malloc(sizeof(Book_Tree));
	newnode->Book[1]=bptr;
	newnode->link[0]=root;
	newnode->link[1]=child;
	newnode->count=1;
	return newnode;
}

void insertbooknode(Book_Info* bptr, int pos, Book_Tree* node, Book_Tree* child)
{
	int j=node->count;
	while(j>pos)
	{
		node->Book[j+1]=node->Book[j];
		node->link[j+1]=node->link[j];
		j--;
	}
	node->Book[j+1]=bptr;
	node->link[j+1]=child;
	(node->count)++;
}

void splitbooknode(Book_Info* new_book, Book_Info* bptr, int pos, Book_Tree* node,
				Book_Tree* child, Book_Tree** newnode)
{
	int median, j;
	
	if(pos>Min)
	{
		median = Min+1;
	}
	else
	{
		median = Min;
	}
	*(newnode)=(Book_Tree*)malloc(sizeof(Book_Tree));
	j=median+1;
	while(j<=Max)
	{
		(*newnode)->Book[j - median] = node->Book[j];
    	(*newnode)->link[j - median] = node->link[j];
    	j++;
	}
	node->count = median;
  	(*newnode)->count = Max - median;
	if(pos<Min)
	{
		insertbooknode(new_book, pos, node, child);
	}
	else
	{
		insertbooknode(new_book, pos-median, *newnode, child);
	}
	bptr = node->Book[node->count];
  	(*newnode)->link[0] = node->link[node->count];
  	node->count--;
}

Boolean setbooknode(Book_Info* new_book, Book_Info* bptr, Book_Tree* node, Book_Tree** child)
{
	int pos,i=1;
	Boolean flag=False , found = False;
	if(!node)
	{
		bptr=new_book;
		*child=NULL;
		flag = True;
	}
	while(i<=node->count && !flag && !found)
	{
		if(string_cmp(node->Book[i]->Subject,new_book->Subject)>0 && !flag)
		{
			pos=i-1;
			found = True;
		}
		else if(string_cmp(node->Book[i]->Subject,new_book->Subject)==0 && !flag)
		{
			if(string_cmp(node->Book[i]->Book_name,new_book->Book_name)>0 && !flag)
			{
				pos=i-1;
				found = True;
			}
			else if(string_cmp(node->Book[i]->Book_name,new_book->Book_name)==0 && !flag)
			{
				if(string_cmp(node->Book[i]->Author_name,new_book->Author_name)>0 && !flag)
				{
					pos=i-1;
					found = True;
				}
				else if(string_cmp(node->Book[i]->Author_name,new_book->Author_name)==0 && !flag)
				{
					if(node->Book[i]->print_year > new_book->print_year && !flag)
					{
						pos=i-1;
						found = True;
					}
					else if(!flag)
					{
						i++;
					}
				}
				else if(!flag)
				{
					i++;
				}
			}
			else if(!flag)
			{
				i++;
			}	
		}
		else if(!flag)
		{
			i++;
		}
	}
	if(!found)
	{
		pos=node->count;
	}
	if(!flag && setbooknode(new_book, bptr, node->link[pos], child))
	{
		if(node->count<Max)
		{
			insertbooknode(bptr, pos, node, *child);
		}
		else
		{
			splitbooknode(bptr, bptr, pos, node, *child, child);
			flag = True;
		}
	}
	return flag;
}

void insertbook(Book_Info* new_book, Book_Tree** root)
{
	Boolean flag;
	Book_Info* bptr;
	Book_Tree* child;
	
	flag = setbooknode(new_book, bptr, *root, &child);
	if(flag)
	{
		*root = createbookroot(bptr, *root, child);
	}
}

// Question No. 3// Question No. 3
// Creating a Member node
Member* create_Member()
{
	Member* mptr=(Member*)malloc(sizeof(Member));
	printf("Enter Name, Department, Status");
	gets(mptr->Name);
	scanf("%s",&mptr->Dept);
	scanf("%s",&mptr->status);
	mptr->fine_no=0;
	No_of_profs++;
	mptr->member_id=No_of_profs;							
	mptr->Fine=0.0;
	mptr->borrowed_no=0;
	mptr->iptr=NULL;
	return mptr;
}

// Functions to insert a member node created in above function in the Tree of Members

Member_Tree* creatememberroot(Member* bptr, Member_Tree* root, Member_Tree* child)
{
	Member_Tree* newnode;
	newnode = (Member_Tree*)malloc(sizeof(Member_Tree));
	newnode->member[1]=bptr;
	newnode->link[0]=root;
	newnode->link[1]=child;
	newnode->count=1;
	return newnode;
}

void insertmembernode(Member* bptr, int pos, Member_Tree* node, Member_Tree* child)
{
	int j=node->count;
	while(j>pos)
	{
		node->member[j+1]=node->member[j];
		node->link[j+1]=node->link[j];
		j--;
	}
	node->member[j+1]=bptr;
	node->link[j+1]=child;
	(node->count)++;
}

void splitmembernode(Member* new_member, Member* bptr, int pos, Member_Tree* node,
				Member_Tree* child, Member_Tree** newnode)
{
	int median, j;
	
	if(pos>Min)
	{
		median = Min+1;
	}
	else
	{
		median = Min;
	}
	*(newnode)=(Member_Tree*)malloc(sizeof(Member_Tree));
	j=median+1;
	while(j<=Max)
	{
		(*newnode)->member[j - median] = node->member[j];
    	(*newnode)->link[j - median] = node->link[j];
    	j++;
	}
	node->count = median;
  	(*newnode)->count = Max - median;
	if(pos<Min)
	{
		insertmembernode(new_member, pos, node, child);
	}
	else
	{
		insertmembernode(new_member, pos-median, *newnode, child);
	}
	bptr = node->member[node->count];
  	(*newnode)->link[0] = node->link[node->count];
  	node->count--;
}

Boolean setmembernode(Member* new_member, Member* bptr, Member_Tree* node, Member_Tree** child)
{
	int pos,i=1;
	Boolean flag=False , found = False;
	if(!node)
	{
		bptr=new_member;
		*child=NULL;
		flag = True;
	}
	while(i<=node->count && !flag)
	{
		if(string_cmp(node->member[i]->Dept,new_member->Dept)>0 && !flag)
		{
			pos=i-1;
			found=True;
		}
		else if(string_cmp(node->member[i]->Dept,new_member->Dept)==0 && !flag)
		{
			if(node->member[i]->member_id>new_member->member_id)
			{
				pos=i-1;
				found=True;
			}
			else if(!flag)
			{
				i++;
			}
		}
		else if(!flag)
		{
			i++;
		}
	}
	if(!found)
	{
		pos= node->count;
	}
	if(!flag && setmembernode(new_member, bptr, node->link[pos], child))
	{
		if(node->count<Max)
		{
			insertmembernode(bptr, pos, node, *child);
		}
		else
		{
			splitmembernode(bptr, bptr, pos, node, *child, child);
			flag = True;
		}
	}
	return flag;
}

void insertmember(Member* new_member, Member_Tree** root,Boolean (*set)(Member*, Member*, Member_Tree*, Member_Tree**))
{
	Boolean flag;
	Member* bptr;
	Member_Tree* child;
	
	flag = set(new_member, bptr, *root, &child);
	if(flag)
	{
		*root = creatememberroot(bptr, *root, child);
	}
}

// Question No. 4
/* Function to print names of member who borrowed maximum number of books*/
int max(Member_Tree* node, int count)
{
	int i;
	if(!node)
	{
		for(i=0;i<node->count;i++)
		{
			max(node->link[i],count);
			if(count<node->member[i+1]->borrowed_no)
			{
				count=node->member[i+1]->borrowed_no;
			}
		}
		max(node->link[i],count);
	}
	return count;
}

void max_borrows_name(Member_Tree* node, int count)
{
	int i;
	if(!node)
	{
		for(i=0;i<node->count;i++)
		{
			max_borrows_name(node->link[i],count);
			if(count==node->member[i+1]->borrowed_no)
			{
				puts(node->member[i+1]->Name);
			}
		}
		max_borrows_name(node->link[i],count);
	}
}

void Max_Borrows(Member_Tree* Faculty, Member_Tree* Student)
{
	int count=0;
	count = max(Faculty,count);
	max_borrows_name(Faculty,count);
	count=0;
	count = max(Student,count);
	max_borrows_name(Student,count);
}

// Question No. 5
typedef struct date_tag
{
	int dd, mm, yy;
}date;

int count_leap_year(yy,mm)
{
	int years=yy;
	if((mm<=2)&&((yy%4==0)||((yy%100==0)&&(yy%400==0))))
	{
		years--;
	}
	years=years/4-years/100+years/400;
	return years;
}

int get_diff_days(Issue* iptr, date date_t)
{
	int diff,i;
	int Monthdays[]={31,28,31,30,31,30,31,31,30,31,30,31};
	long int n1=iptr->R_yy*365+iptr->R_dd;
	for(i=0;i<iptr->R_mm-1;i++)
	{
		n1+=Monthdays[i];	
	}
	n1+=count_leap_year(iptr->R_yy,iptr->R_mm);
	
	long int n2=date_t.yy*365+date_t.dd;
	for(i=0;i<date_t.mm-1;i++)
	{
		n2+=Monthdays[i];
	}
	n2+=count_leap_year(date_t.yy,date_t.mm);
	
	diff=n1-n2;
	if(diff<0)
	{
		diff=0;
	}
	return diff;
}

float max_fine(Member_Tree* node, float fine, date date_t)
{
	int i;
	int over_days;
	if(!node)
	{
		for(i=0;i<node->count;i++)
		{
			max_fine(node->link[i],fine,date_t);
			over_days=get_diff_days(node->member[i+1]->iptr,date_t);
			node->member[i+1]->Fine=over_days*0.5;
			if(node->member[i+1]->Fine>fine)
			{
				fine=node->member[i+1]->Fine;
			}
			if(node->member[i]->Fine>0)
			{
				(node->member[i]->fine_no)++;
			}
		}
		max_fine(node->link[i],fine,date_t);
	}
	return fine;
}

void max_fine_name(Member_Tree* node, int fine)
{
	int i;
	if(!node)
	{
		for(i=0;i<node->count;i++)
		{
			max_fine_name(node->link[i],fine);
			if(fine==node->member[i+1]->Fine)
			{
				puts(node->member[i+1]->Name);
			}
		}
		max_fine_name(node->link[i],fine);
	}
}

void Max_Fine(Member_Tree* Faculty, Member_Tree* Student)
{
	float fine=0;
	date today_date;
	printf("Enter today's date in dd mm yyyy format\t");
	scanf("%d,%d,%d",&today_date.dd,&today_date.mm,today_date.yy);
	
	fine=max_fine(Faculty,fine,today_date);
	fine=max_fine(Student,fine,today_date);
	max_fine_name(Faculty,fine);
	max_fine_name(Student,fine);
}

// Question No. 6
// Function to sort the member using no of borrowers and displaying their names in descending order
Boolean setmembernode_sort(Member* new_member, Member* bptr, Member_Tree* node, Member_Tree** child)
{
	int pos,i=1;
	Boolean flag=False , found = False;
	if(!node)
	{
		bptr=new_member;
		*child=NULL;
		flag = True;
	}
	while(i<=node->count && !flag)
	{
		if(node->member[i]->borrowed_no>new_member->borrowed_no && !flag)
		{
			pos=i-1;
			found=True;
		}
		else if(node->member[i]->borrowed_no==new_member->borrowed_no && !flag)
		{
			if((node->member[i])->status[0]=='S' && (new_member)->status[0]=='P' && !flag)
			{
				pos=i-1;
				found=True;
			}
			else if(!flag)
			{
				i++;
			}
		}
		else if(!flag)
		{
			i++;
		}
	}
	if(!found)
	{
		pos= node->count;
	}
	if(!flag && setmembernode(new_member, bptr, node->link[pos], child))
	{
		if(node->count<Max)
		{
			insertmembernode(bptr, pos, node, *child);
		}
		else
		{
			splitmembernode(bptr, bptr, pos, node, *child, child);
			flag = True;
		}
	}
	return flag;
}

void copy_member_node(Member* node, Member* source)
{
	node->borrowed_no=source->borrowed_no;
	node->Fine=source->Fine;
	node->fine_no=source->fine_no;
	node->member_id=source->member_id;
	node->iptr=source->iptr;
	string_copy(node->Dept,source->Dept);
	string_copy(node->Name,source->Name);
	string_copy(node->status,source->status);
}

void set(Member_Tree** root, Member_Tree* node)
{
	int i;
	Member* new_member;
	new_member=(Member*)malloc(sizeof(Member));
	if(!node)
	{
		for(i=0;i<node->count;i++)
		{
			set(root,node->link[i]);
			copy_member_node(new_member,node->member[i+1]);
			insertmember(new_member,root, &setmembernode_sort);
		}
		set(root,node->link[i]);
	}
}

void print_names_descending(Member_Tree* node)
{
	int i=node->count;
	if(!node)
	{
		print_names_descending(node->link[i]);
		i--;
		while(i>=0)
		{
			puts(node->member[i+1]->Name);
			print_names_descending(node->link[i]);
			i--;
		}
	}
}

void sort(Member_Tree* Faculty, Member_Tree* Student)
{
	int i;
	Member_Tree* Sorted=NULL;
	set(&Sorted,Faculty);
	set(&Sorted, Student);
	print_names_descending(Sorted);
}

// Question No. 7
// Sort according to no of times paid fine
typedef struct Member_B_Tree_node
{
	int count;
	Member *member[MAX+1];
	struct Member_B_Tree_node *link[MAX+1];	
}Member_B_Tree;

Member_B_Tree* creatememberrootB(Member* bptr, Member_B_Tree* root, Member_B_Tree* child)
{
	Member_B_Tree* newnode;
	newnode = (Member_B_Tree*)malloc(sizeof(Member_B_Tree));
	newnode->member[1]=bptr;
	newnode->link[0]=root;
	newnode->link[1]=child;
	newnode->count=1;
	return newnode;
}

void insertmembernodeB(Member* bptr, int pos, Member_B_Tree* node, Member_B_Tree* child)
{
	int j=node->count;
	while(j>pos)
	{
		node->member[j+1]=node->member[j];
		node->link[j+1]=node->link[j];
		j--;
	}
	node->member[j+1]=bptr;
	node->link[j+1]=child;
	(node->count)++;
}

void splitmembernodeB(Member* new_member, Member* bptr, int pos, Member_B_Tree* node,
				Member_B_Tree* child, Member_B_Tree** newnode)
{
	int median, j;
	
	if(pos>MIN)
	{
		median = MIN+1;
	}
	else
	{
		median = MIN;
	}
	*(newnode)=(Member_B_Tree*)malloc(sizeof(Member_B_Tree));
	j=median+1;
	while(j<=MAX)
	{
		(*newnode)->member[j - median] = node->member[j];
    	(*newnode)->link[j - median] = node->link[j];
    	j++;
	}
	node->count = median;
  	(*newnode)->count = MAX - median;
	if(pos<MIN)
	{
		insertmembernodeB(new_member, pos, node, child);
	}
	else
	{
		insertmembernodeB(new_member, pos-median, *newnode, child);
	}
	bptr = node->member[node->count];
  	(*newnode)->link[0] = node->link[node->count];
  	node->count--;
}

Boolean setmembernodeB(Member* new_member, Member* bptr, Member_B_Tree* node, Member_B_Tree** child)
{
	int pos,i=1;
	Boolean flag=False , found = False;
	if(!node)
	{
		bptr=new_member;
		*child=NULL;
		flag = True;
	}
	while(i<=node->count && !flag)
	{
		if(node->member[i]->fine_no>new_member->fine_no && !flag)
		{
			pos=i-1;
			found=True;
		}
		else if(!flag)
		{
			i++;
		}
	}
	if(!found)
	{
		pos= node->count;
	}
	if(!flag && setmembernodeB(new_member, bptr, node->link[pos], child))
	{
		if(node->count<Max)
		{
			insertmembernodeB(bptr, pos, node, *child);
		}
		else
		{
			splitmembernodeB(bptr, bptr, pos, node, *child, child);
			flag = True;
		}
	}
	return flag;
}

void insertmemberB(Member* new_member, Member_B_Tree** root)
{
	Boolean flag;
	Member* bptr;
	Member_B_Tree* child;
	
	flag = setmembernodeB(new_member, bptr, *root, &child);
	if(flag)
	{
		*root = creatememberrootB(bptr, *root, child);
	}
}

void deletemembertreeB(Member_B_Tree** root)
{
	int i;
	if(!(*root))
	{
		for(i=0;i<(*root)->count;i++)
		{
			deletemembertreeB(&(*root)->link[i]);
			free((*root)->member[i+1]);
			(*root)->member[i+1]=NULL;
		}
		deletemembertreeB(&((*root)->link[i]));
		free((*root));
		*root=NULL;
	}
}

void Sort_B(Member_B_Tree* root, Member_Tree* node)
{
	int i;
	Member* new_member;
	new_member=(Member*)malloc(sizeof(Member));
	if(!node)
	{
		for(i=0;i<node->count;i++)
		{
			Sort_B(root,node->link[i]);
			if(node->member[i+1]->fine_no>0)
			{
				copy_member_node(new_member,node->member[i+1]);
				insertmemberB(new_member,&root);
			}
		}
		Sort_B(root,node->link[i]);
	}
}

void print_names_B_Tree(Member_B_Tree* node)
{
	int i;
	if(!node)
	{
		for(i=0;i<node->count;i++)
		{
			print_names_B_Tree(node->link[i]);
			if(node->member[i+1]->fine_no>2)
			{
				puts(node->member[i+1]->Name);
			}
			print_names_B_Tree(node->link[i]);
		}
	}
}

void sort_B_Tree(Member_Tree* Faculty, Member_Tree* Student)
{
	Member_B_Tree* root = NULL;
	Sort_B(root, Faculty);
	Sort_B(root, Student);
	print_names_B_Tree(root);
	deletemembertreeB(&root);
}

int main()
{
	Library Lib_DB;
	int i,no_books,no_members;
	Initialize_DB(&Lib_DB);
	Book_Info* bptr;
	Member* mptr;
	printf("Enter no of books to be inserted");
	scanf("%d",&no_books);
	for(i=0;i<no_books;i++)
	{
		bptr=create_book();
		insertbook(bptr,&(Lib_DB.bptr));
	}
	printf("Enter no of Faculty to be inserted");
	scanf("%d",&no_members);
	for(i=0;i<no_members;i++)
	{
		mptr=create_Member();
		insertmember(mptr,&(Lib_DB.Faculty), &setmembernode);
	}
	printf("Enter no of Students to be inserted");
	scanf("%d",&no_members);
	for(i=0;i<no_members;i++)
	{
		mptr=create_Member();
		insertmember(mptr,&(Lib_DB.Student), &setmembernode);
	}
	Max_Borrows(Lib_DB.Faculty, Lib_DB.Student);
	Max_Fine(Lib_DB.Faculty, Lib_DB.Student);
	sort(Lib_DB.Faculty, Lib_DB.Student);
	sort_B_Tree(Lib_DB.Faculty, Lib_DB.Student);
	deletebooktree(&(Lib_DB.bptr));
	deletemembertree(&(Lib_DB.Faculty));
	deletemembertree(&(Lib_DB.Student));
}

