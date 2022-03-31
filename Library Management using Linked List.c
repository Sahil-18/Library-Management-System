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
#define Book_DB_Size 1000
#define Member_DB_Size 850
#define Max_Rack 4					// Maximum number of racks in a hall
#define Max_Row 5					// Maximum number of rows in one rack
#define Max_Books 10				// Maximum number of books in a row
typedef enum {Failure, Success} Status_code;
typedef enum {Non_Issued,Issued} Issue_status;

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
	struct Book_Info_node* next;
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
	float Fine;						// Fine if book returned late or 0;
	Issue* iptr;
	struct Member_tag* next;
}Member;

typedef struct Library_tag
{
	Book_Info* bptr;
	Member* Student;
	Member* Faculty;
}Library;

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

// Initialize Library Structure
void Initialize_DB(Library* lptr)
{
	lptr->bptr=NULL;
	lptr->Faculty=NULL;
	lptr->Student=NULL;
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
	/* Default values of following parameters
	They are changed according to the number of books and their sorted order*/
	bptr->accession_no=0;
	bptr->status=0;
	bptr->next=NULL;
	(bptr->Loc).hall_no=0;
	(bptr->Loc).rack_no=0;
	(bptr->Loc).row_no=0;
	(bptr->Loc).book_no=0;	
}
// Change Location
void ChangeLoc(Book_Info* bptr)
{
	Book_Info* ptr=bptr;
	while(ptr!=NULL)
	{
		ptr->accession_no+=1;
		if((ptr->Loc).book_no==Max_Books)
		{
			(ptr->Loc).book_no=1;
			if((ptr->Loc).row_no==Max_Row)
			{
				(ptr->Loc).row_no=1;
				if((ptr->Loc).rack_no==Max_Rack)
				{
					(ptr->Loc).rack_no=1;
					(ptr->Loc).hall_no+=1;
				}
				else
				{
					(ptr->Loc).rack_no+=1;
				}
			}
			else
			{
				(ptr->Loc).row_no+=1;
			}
		}
		else
		{
			(ptr->Loc).book_no+=1;
		}
		ptr=ptr->next;
	}	
}

// Insert node q after node p
Status_code insertafter(Book_Info* q, Book_Info* p, Book_Info** bpptr)
{
	Status_code ret_val=Success;
	Book_Info* ptr;
	if(q==NULL)
	{
		ret_val=Failure;
	}
	else if((p==NULL)&&(*bpptr==NULL))
	{
		*bpptr=q;
		q->next=NULL;
		q->accession_no=1;
		(q->Loc).hall_no=1;
		(q->Loc).rack_no=1;
		(q->Loc).row_no=1;
		(q->Loc).book_no=1;
	}
	else if(p==NULL)
	{
		q->next=*bpptr;
		q->accession_no=1;
		(q->Loc).hall_no=1;
		(q->Loc).rack_no=1;
		(q->Loc).row_no=1;
		(q->Loc).book_no=1;
		ptr=*bpptr;
		*bpptr=q;
		ChangeLoc(ptr);
	}
	else
	{
		q->next=p->next;
		p->next=q;
		ptr=q->next;
		q->accession_no=ptr->accession_no;
		(q->Loc).hall_no=(ptr->Loc).hall_no;
		(q->Loc).rack_no=(ptr->Loc).rack_no;
		(q->Loc).row_no=(ptr->Loc).row_no;
		(q->Loc).book_no=(ptr->Loc).book_no;
		ChangeLoc(ptr);
	}
	return ret_val;
}

/* To insert new book in record
Here I am assumed that Book Data is already sorted. And we have to add new book at the proper place according to sorting conditions given */

Book_Info* insert_book(Book_Info* bptr)
{
	Book_Info *new_ptr,*prev,*ptr=bptr;
	Status_code flag=Failure,sc;
	new_ptr=create_book();
	prev=NULL;
	if(bptr==NULL)
	{
		sc=insertafter(new_ptr,prev,&bptr);
	}
	else
	{
		while((ptr->next!=NULL)&&(!flag))
		{
			// Comparing the subject of books
			if(string_cmp(bptr->Subject,new_ptr->Subject)<0)		// Subject of new book is alphabetically greater, hence moving to next book
			{
				prev=ptr;
				ptr=ptr->next;
			}
			else if(string_cmp(bptr->Subject,new_ptr->Subject)>0)	// Subject of new book is alphabetically smaller i.e. no subject book found
			{
				flag=Success;
			}
			else
			{
				// Subjects are same, now comparing the Book name
				if(string_cmp(bptr->Book_name,new_ptr->Book_name)<0)
				{
					prev=ptr;
					ptr=ptr->next;
				}
				else if(string_cmp(bptr->Book_name,new_ptr->Book_name)>0)
				{
					flag=Success;
				}
				else
				{
					// When Subject and Book name are same, comparing the Author name
					if(string_cmp(bptr->Author_name,new_ptr->Author_name)<0)
					{
						prev=ptr;
						ptr=ptr->next;
					}
					else if(string_cmp(bptr->Author_name,new_ptr->Author_name)>0)
					{
						flag=Success;
					}
					else
					{
						// Subject, Book name and Author name are same then comparing print year
						if(bptr->print_year < new_ptr->print_year)
						{
							prev=ptr;
							ptr=ptr->next;
						}
						else
						{
							flag=Success;
						}
					}
				}
			}
		}
		if(!flag)
		{
			// Comparing the subject of books
			if(string_cmp(bptr->Subject,new_ptr->Subject)<0)		// Subject of new book is alphabetically greater, hence moving to next book
			{
				prev=ptr;
				ptr=ptr->next;
			}
			else if(string_cmp(bptr->Subject,new_ptr->Subject)>0)	// Subject of new book is alphabetically smaller i.e. no subject book found
			{
				flag=Success;
			}
			else
			{
				// Subjects are same, now comparing the Book name
				if(string_cmp(bptr->Book_name,new_ptr->Book_name)<0)
				{
					prev=ptr;
					ptr=ptr->next;
				}
				else if(string_cmp(bptr->Book_name,new_ptr->Book_name)>0)
				{
					flag=Success;
				}
				else
				{
					// When Subject and Book name are same, comparing the Author name
					if(string_cmp(bptr->Author_name,new_ptr->Author_name)<0)
					{
						prev=ptr;
						ptr=ptr->next;
					}
					else if(string_cmp(bptr->Author_name,new_ptr->Author_name)>0)
					{
						flag=Success;
					}
					else
					{
						// Subject, Book name and Author name are same then comparing print year
						if(bptr->print_year < new_ptr->print_year)
						{
							prev=ptr;
							ptr=ptr->next;
						}
						else
						{
							flag=Success;
						}
					}
				}
			}	
		}
		sc=insertafter(new_ptr,prev,&bptr);
	}
	return bptr;
}

// Question No. 3
// Creating a Member node
Member* create_Member()
{
	Member* mptr=(Member*)malloc(sizeof(Member));
	printf("Enter Name, Department, Status");
	gets(mptr->Name);
	scanf("%s",&mptr->Dept);
	scanf("%s",&mptr->status);
	mptr->member_id=0;							// Default, can be change at the time of insertion in linked list
	mptr->Fine=0.0;
	mptr->borrowed_no=0;
	mptr->iptr=NULL;
	mptr->next=NULL;
	return mptr;
}

// To insert new member in record
void insert_member(Member** Faculty, Member** Student)
{
	Member* ptr;
	Member* new_ptr;
	new_ptr=create_Member();
	if(new_ptr->status[1]='P')
	{
		if(*Faculty==NULL)
		{
			*Faculty=new_ptr;
			new_ptr->member_id=1;
		}
		else
		{
			ptr=*Faculty;
			while(ptr->next!=NULL)
			{
				ptr=ptr->next;
			}
			new_ptr->member_id=ptr->member_id+1;
			ptr->next=new_ptr;
		}
	}
	else
	{
		if(*Student==NULL)
		{
			*Student=new_ptr;
			new_ptr->member_id=1;
		}
		else
		{
			ptr=*Student;
			while(ptr->next!=NULL)
			{
				ptr=ptr->next;
			}
			new_ptr->member_id=ptr->member_id+1;
			ptr->next=new_ptr;
		}
	}
}

// Question No. 4
/* Function to print names of member who borrowed maximum number of books*/
void Max_borrows(Member* Faculty, Member* Student)
{
	int count=0,Max_prof=4,Max_stud=2;
	Member* ptr;
	ptr=Faculty;
	while((ptr->next!=NULL)&&(count<Max_prof))
	{
		if(ptr->borrowed_no>count)
		{
			count=ptr->borrowed_no;
		}
		ptr=ptr->next;
	}
	if(ptr->next==NULL)
	{
		if(ptr->borrowed_no>count)
		{
			count=ptr->borrowed_no;
		}
	}
	ptr=Faculty;
	while(ptr!=NULL)
	{
		if(ptr->borrowed_no==count)
		{
			puts(ptr->Name);
		}
		ptr=ptr->next;
	}
	ptr=Student;
	while((ptr->next!=NULL)&&(count<Max_stud))
	{
		if(ptr->borrowed_no>count)
		{
			count=ptr->borrowed_no;
		}
		ptr=ptr->next;
	}
	if(ptr->next==NULL)
	{
		if(ptr->borrowed_no>count)
		{
			count=ptr->borrowed_no;
		}
	}
	ptr=Student;
	while(ptr!=NULL)
	{
		if(ptr->borrowed_no==count)
		{
			puts(ptr->Name);
		}
		ptr=ptr->next;
	}
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

void Max_fine(Member** Faculty, Member** Student)
{
	float max=0;
	int over_days;
	date today_date;
	Member* ptr;
	Issue* iptr;
	
	printf("Enter today's date in dd mm yyyy format\t");
	scanf("%d,%d,%d",&today_date.dd,&today_date.mm,today_date.yy);
	
	ptr=*Faculty;
	while(ptr=NULL)
	{
		iptr=ptr->iptr;
		while(iptr!=NULL)
		{
			over_days=get_diff_days(iptr,today_date);
			ptr->Fine+=(over_days*0.5);
			iptr=iptr->next;
		}
		if(max<ptr->Fine)
		{
			max=ptr->Fine;
		}
		ptr=ptr->next;
	}
	ptr=*Student;
	while(ptr=NULL)
	{
		iptr=ptr->iptr;
		while(iptr!=NULL)
		{
			over_days=get_diff_days(iptr,today_date);
			ptr->Fine+=(over_days*0.5);
			iptr=iptr->next;
		}
		if(max<ptr->Fine)
		{
			max=ptr->Fine;
		}
		ptr=ptr->next;
	}
	ptr=*Faculty;
	while(ptr!=NULL)
	{
		if(ptr->Fine==max)
		{
			puts(ptr->Name);
		}
	}
	ptr=*Student;
	while(ptr!=NULL)
	{
		if(ptr->Fine==max)
		{
			puts(ptr->Name);
		}
	}	
}

// Question No. 6
// Function to display name of members who borrowed books in descending order according to given conditions
typedef struct name_tag
{
	char Name[Name_Len];
	int borrowd_no;
	struct name_tag* next;
}name;

void sort_display(Member* mem, name** head)
{	
	Status_code flag;
	Member *p=mem;
	name* new_ptr=(name*)malloc(sizeof(name));
	name *ptr,*prev;
	while(p!=NULL)
	{	
		string_copy(new_ptr->Name,p->Name);
		new_ptr->borrowd_no=p->borrowed_no;
		prev=NULL;
		ptr=*head;
		flag=Success;
		if(ptr==NULL)
		{
			*head=new_ptr;
			new_ptr->next=NULL;
		}
		else
		{
			// The linked list of names of borrowers is sorted
			while((ptr->next!=NULL)&&(!flag))
			{
				if(ptr->borrowd_no<p->borrowed_no)
				{
					prev=ptr;
					ptr=ptr->next;
				}
				else if(ptr->borrowd_no>p->borrowed_no)
				{
					flag=Success;
				}
				else
				{
					if((ptr->Name[0]=='P')&&(p->Name[0]=='S'))
					{
						prev=ptr;
						flag=Success;
					}
					else if((ptr->Name[0]=='S')&&(p->Name[0]=='P'))
					{
						flag=Success;
					}
					else
					{
						if(string_cmp(ptr->Name,p->Name)<0)
						{
							prev=ptr;
							ptr=ptr->next;
						}
						else
						{
							flag=Success;
						}
					}
				}
			}
			if(!flag)
			{
				if(ptr->borrowd_no<p->borrowed_no)
				{
					prev=ptr;
				}
				else if(ptr->borrowd_no>p->borrowed_no)
				{
					flag=Success;
				}
				else
				{
					if((ptr->Name[0]=='P')&&(p->Name[0]=='S'))
					{
						prev=ptr;
						flag=Success;
					}
					else if((ptr->Name[0]=='S')&&(p->Name[0]=='P'))
					{
						flag=Success;
					}
					else
					{
						if(string_cmp(ptr->Name,p->Name)<0)
						{
							prev=ptr;
						}
						else
						{
							flag=Success;
						}
					}
				}	
			}
			new_ptr->next=prev->next;
			prev->next=new_ptr;
			p=p->next;
		}
	}
	free(new_ptr);
}

void Sort(Member* Faculty, Member* Student, name** head)
{
	sort_display(Faculty,head);
	sort_display(Student,head);
}

void main()
{
	Library Lib_DB;
	Status_code sc;
	name *nptr=NULL,*ptr,**npptr;
	npptr=&nptr;
	int i,no_books,no_mem;
	Initialize_DB(&Lib_DB);
	printf("Enter no of books to be inserted");
	scanf("%d",&no_books);
	for(i=0;i<no_books;i++)
	{
		Lib_DB.bptr=insert_book(Lib_DB.bptr);
	}
	printf("Enter no of books to be inserted");
	scanf("%d",&no_mem);
	for(i=0;i<no_mem;i++)
	{
		insert_member(&Lib_DB.Faculty,&Lib_DB.Student);
	}
	Max_borrows(Lib_DB.Faculty,Lib_DB.Student);
	Max_fine(&Lib_DB.Faculty,&Lib_DB.Student);
	Sort(Lib_DB.Faculty,Lib_DB.Student,npptr);
	ptr=nptr;
	while(ptr!=NULL)
	{
		puts(ptr->Name);
	}
}
