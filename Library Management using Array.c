/* My Enrollment Number is BT19CSE088. 
Hence I am solving problem for R3 Batch i.e Library System */

#include<stdio.h>
#define Name_Len 100
#define Book_Name_Len 100
#define Sub_Len 40
#define Dep_Len 50
#define Stat_Len 15
#define Book_DB_Size 1000
#define Member_DB_Size 850
#define Success 1
#define Failure 0


// Question No 1:

// Structure defining location in the Library in terms of hall, rack and row
struct Loc
{
	int Hall_no, Rack_no,Row_no;
};

// Structure defining book information
struct Book_info
{
	char Book_name[Book_Name_Len];
	char Subject[Sub_Len];
	char Author_name[Name_Len];
	int accession_no;				// A unique number associated with every book
	int print_year;					// Year in which the book is printed
	int status;    					// Non Issued == 1, Issued == 0
};

// Struction defining the information about Book Issued 
struct Issue
{
	int I_dd,I_mm,I_yy; // Date of Issue/Renewal
	int R_dd,R_mm,R_yy; // Date of Return 
	int no_renew;	    // No of times book renewed, a book can be maximum 3 times renewed
	int accession_no;   // Accession number of Book borrowed
};


struct Member
{
	char Name[Name_Len];
	char Department[Dep_Len];
	char status[Stat_Len];			// Either Professor or Student
	int member_id;					
	int borrowed_no;				// Students can borrow max 2 book and Professors can borrow max 4 books at a time
	float Fine;   					// Fine if book returned late or 0
	struct Issue Issue_info[4];		// Array of books borrowed by member
};


struct Library
{
	struct Book_info Book_DB[Book_DB_Size];
	struct Loc Loc_info[Book_DB_Size];
	struct Member Member_DB[Member_DB_Size];
};

// Initializing the structure Library
void Initialize_DB(struct Library Lib_DB, int book_data_size, int member_data_size)
{
	int i,j,hall_no=1,rack_no=1,row_no=1,count=0;
	for(i=0;i<book_data_size;i++)
	{
		Lib_DB.Book_DB[i].Book_name[0]='\0';
		Lib_DB.Book_DB[i].Subject[0]='\0';
		Lib_DB.Book_DB[i].Author_name[0]='\0';
		Lib_DB.Book_DB[i].accession_no=0;
		Lib_DB.Book_DB[i].print_year=0;
		Lib_DB.Book_DB[i].status=1;
		if(rack_no<=4) 									// One hall contains 4 racks
		{
			if(row_no<=5)     							// one rack contains 5 rows
			{	
				if(count<10)      						// one row contains 10 books
				{	Lib_DB.Loc_info[i].Hall_no=hall_no;
					Lib_DB.Loc_info[i].Rack_no=rack_no;
					Lib_DB.Loc_info[i].Row_no=row_no;
					count=count+1;
				}
				else
				{
					count=0;
					row_no=row_no+1;
				}
			}
			else
			{
				row_no=1;
				rack_no=rack_no+1;
			}
		}
		else
		{
			rack_no=1;
			hall_no=hall_no+1;
		}
	}
	
	for(i=0;i<member_data_size;i++)
	{
		Lib_DB.Member_DB[i].Name[0]='\0';
		Lib_DB.Member_DB[i].Department[0]='\0';
		Lib_DB.Member_DB[i].status[0]='\0';
		Lib_DB.Member_DB[i].member_id=0;
		Lib_DB.Member_DB[i].borrowed_no=0;
		for(j=0;j<4;j++)
		{
			Lib_DB.Member_DB[i].Issue_info[j].I_dd=0;
			Lib_DB.Member_DB[i].Issue_info[j].I_mm=0;
			Lib_DB.Member_DB[i].Issue_info[j].I_yy=0;
			Lib_DB.Member_DB[i].Issue_info[j].R_dd=0;
			Lib_DB.Member_DB[i].Issue_info[j].R_mm=0;
			Lib_DB.Member_DB[i].Issue_info[j].R_yy=0;
			Lib_DB.Member_DB[i].Issue_info[j].accession_no=0;
		}
		Lib_DB.Member_DB[i].Fine=0;
	}
}


// Question No 2:

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

/* To insert new book record.
No_insert is equal to number of book records you want to insert.*/
// Here I assumed that Data in the Library structure is sorted.

int insert_book(struct Library Lib, int book_data_size, int book_name_len, int name_len, int sub_len, int No_insert)
{
	int i,j,k,flag,pos,print_year,total_records=0,status=Success;
	char book_name[book_name_len],author_name[name_len],subject[sub_len];
		
	while(Lib.Book_DB[total_records].Book_name[0]!='\0')
	{
		total_records=total_records+1;
	}
		
	for(k=0;k<No_insert;k++)
	{	
		i=0;
		flag=0;
		
		// Checking availability of memory to accommodate new data
		 
		if(total_records==book_data_size)	// No memory available
		{
			status=Failure;
			printf("Can't insert further %d records\n",No_insert-k);
		}
		else
		{
			// Memory available. Hence taking input of new book data
			
			printf("Enter Book name\t");
			scanf(" %s",&book_name);
			printf("Enter Subject\t");
			scanf(" $s",&subject);
			printf("Enter Author name\t");
			scanf(" %s",&author_name);
			printf("Enter print year\t");
			scanf("%d",&print_year);
			
			// Checking for any book availble in Library similar to the new book
			 
			while((i<total_records)&&(flag==0))
			{	
				// Comparing Subject
				
				if(string_cmp(Lib.Book_DB[i].Subject,subject)<0)		// Subject of new book is alphabetically greater, hence moving to next book in library
				{
					i++;
				}
				else if(string_cmp(Lib.Book_DB[i].Subject,subject)>0)	/* Subject of book is alphabetically smaller i.e. no same subject is found, but books inLibrary are sorted
																			first with respect to Subject. Hence new book will be place in this position */
				{
					pos=i;
					flag=1;
				}
				else
				{	
					// When Subject is Same, then comparing Book Name
					// Logic applied to comparing Subject is applied to all other fields too
					
					if(string_cmp(Lib.Book_DB[i].Book_name,book_name)<0)
	        	    {
	        	        i++;
	        	    }
	        	    else if(string_cmp(Lib.Book_DB[i].Book_name,book_name)>0)
	        	    {
	        	        pos=i;
	        	        flag=1;
	        	    }
	            	else
	            	{
	            		// When both Subject and Book Name are same, then comparing the Author Name
	            		
	                	if(string_cmp(Lib.Book_DB[i].Author_name,author_name)<0)
	                	{	
	                    	i++;
	                	}
	                	else if(string_cmp(Lib.Book_DB[i].Author_name,author_name)>0)
	                	{
	                    	pos=i;
	                    	flag=1;
	                	}
	                	else
	                	{
	                		// When Subject, Book Name and Author Name are same, then comparing year of Print
	                		
	                    	if(Lib.Book_DB[i].print_year<print_year)
	                    	{
	                    	    i++;
	                    	}
	                    	else
	                    	{
	                    	    pos=i;
	                    	    flag=1;
	                    	}
	                	}	
	            	}
	        	}
	        	
	        	if(flag==0)			// No similar book found. Hence appending it to last place of Library
	        	{
	        		pos=i;
				}
				
				// Shifting all records from i th position to end of of Data by one place and placing new data in i th position
				
				for(j<total_records;j<i;j--)
				{
					Lib.Book_DB[j]=Lib.Book_DB[j-1];
				}
				string_copy(Lib.Book_DB[i].Book_name,book_name);
				string_copy(Lib.Book_DB[i].Author_name,author_name);
				string_copy(Lib.Book_DB[i].Subject,subject);
				Lib.Book_DB[i].accession_no=total_records+1;   // Maximum 'Accession Number' in Database is equal to 'Total Number of Books' in Library
				Lib.Book_DB[i].print_year=print_year;
				Lib.Book_DB[i].status=1;		
			}
			// Updating total records in data base
			
			total_records=total_records+1;
			printf("Successfully inserted %d th book data\n",k+1);
		}
	}
	return status;
}



// Question No 3:

/* To insert new member record.
No_insert is equal to number of member records you want to insert.*/

int new_member(struct Library Lib, int member_data_size, int name_len, int dep_len, int stat_len, int No_insert)
{
	int i,total_records=0,status=Success;
	while(Lib.Member_DB[total_records].Name[0]!='\0')
	{
		total_records++;
	}
	for(i=0;i<No_insert;i++)
	{
		// Checking availability of memory to accommodate new data
		
		if(total_records==member_data_size)			// No memory available
		{
			printf("Can not insert further %d records\n",No_insert-i);
			status=Failure;
		}
		else
		{
			// Memory available. Hence taking input of new book data
			
			printf("Enter your name\t");
			scanf(" %s",&Lib.Member_DB[total_records].Name);
			printf("Enter your Department\t");
			scanf(" %s",&Lib.Member_DB[total_records].Department);
			printf("Enter your Status i.e. Student or Professor\t");
			scanf(" %s",&Lib.Member_DB[total_records].status);
			printf("Enter your Id\t");
			scanf("%d",&Lib.Member_DB[total_records].member_id);
		}
		// Updating total number of records
		
		total_records++;
		printf("Successfully inserted % th record\n",i+1);		
	}
	return status;
}



// Question No 4:

void Max_borrow(struct Library Lib, int member_data_size)
{
	int i,count_faculty=0,count_stud,max_faculty=4,max_stud=2,total_records=0;
	// For faculty maximum books borrowed can be 4 but for student it is 2
	
	while(Lib.Member_DB[total_records].Name[0]!='\0')
	{
		total_records++;
	}
	
	while((count_faculty=0)||(count_stud=0))
	{
		for(i=0;i<total_records;i++)
		{
			if(Lib.Member_DB[i].status[0]=='P')
			{
				// If member is Faculty, then compare with maximum books which faculty can borrow
				if(Lib.Member_DB[i].borrowed_no==max_faculty)
				{
					// If equal printing faculty data and incrementing faculty counter
					
					printf("%s\t%s\t%s\t%d\n",Lib.Member_DB[i].Name,Lib.Member_DB[i].Department,Lib.Member_DB[i].status,Lib.Member_DB[i].member_id);
					count_faculty++;
				}
			}
			else if(Lib.Member_DB[i].status[0]=='S')
			{
				// If member is Student, then compare with maximum books which student can borrow
				
				if(Lib.Member_DB[i].borrowed_no==max_stud)
				{
					// If equal printing faculty data and incrementing student counter
					
					printf("%s\t%s\t%s\t%d\n",Lib.Member_DB[i].Name,Lib.Member_DB[i].Department,Lib.Member_DB[i].status,Lib.Member_DB[i].member_id);
					count_stud++;
				}
				
			}
		}
		
		// Faculty counter is Zero and maximum books faculty can borrow is greater than 1, then decrementing maximum books faculty can borrow by 1
		
		if((count_faculty==0)&&(max_faculty>=1))
		{
			if(max_faculty==1)
			{
				// This can help in coming out of loop if no one has borrowed book
				count_faculty++;
			}
			else
			{
				max_faculty--;
			}
		}
		
		// Student counter is Zero and maximum books students can borrow is greater than 1, then decrementing maximum books students can borrow by 1
		
		if((count_stud==0)&&(max_stud>=1))
		{
			if(max_stud==1)
			{
				// This can help in coming out of loop if no one has borrowed book
				count_stud++;
			}
			else
			{
				max_stud--;
			}
		}
	}
}



// Question No 5:
struct date
{
	int dd,mm,yy;
};

int countLeapYears(yy,mm)
{
    int years=yy;
 
    if((mm <= 2)&&((yy%4==0)||((yy%100==0)&&(yy%400==0))))
    {
        years--;
	}
    years= years / 4 - years / 100 + years / 400;
    return years;
}

int get_diff_days(struct Library lib, int member_no, int book_no, struct date date_t)
{
	int diff,i;
	int monthDays[12]= { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	long int n1 = lib.Member_DB[member_no].Issue_info[book_no].R_yy * 365 + lib.Member_DB[member_no].Issue_info[book_no].R_dd;
	for (i = 0; i < lib.Member_DB[member_no].Issue_info[book_no].R_mm - 1; i++)
        n1 += monthDays[i];
    n1 += countLeapYears(lib.Member_DB[member_no].Issue_info[book_no].R_yy,lib.Member_DB[member_no].Issue_info[book_no].R_mm);
    
    long int n2 = date_t.yy*365+date_t.dd;
   	for (i = 0; i < date_t.mm- 1; i++)
        n2 += monthDays[i];
    n2 += countLeapYears(date_t.yy,date_t.mm);
    diff=n1-n2;
    if(diff<0)
    {
    	diff=0;
	}
    return diff;
}

void max_fine(struct Library Lib, int member_data_size)
{
	int i,j,total_records=0,over_days;
	float max=0.0;
	struct date today_date;
	
	printf("Enter today's date in dd mm yyyy format\t");
	scanf("%d,%d,%d",&today_date.dd,&today_date.mm,today_date.yy);
	
	while(Lib.Member_DB[total_records].Name[0]!='\0')
	{
		total_records++;
	}
	
	for(i=0;i<total_records;i++)
	{
		for(j=0;j<Lib.Member_DB[i].borrowed_no;j++)
		{
			over_days=get_diff_days(Lib,i,j,today_date);
			Lib.Member_DB[i].Fine=Lib.Member_DB[i].Fine+over_days*0.5;
		}
		if(Lib.Member_DB[i].Fine>max)
		{
			max=Lib.Member_DB[i].Fine;
		}
	}
	
	if(max>0)
	{
		printf("Following member have maximum fine till date\n");
		for(i=0;i<total_records;i++)
		{
			if(max==Lib.Member_DB[i].Fine);
			{
				printf("%s\t%s\t%s\t%d\t%f\n",Lib.Member_DB[i].Name,Lib.Member_DB[i].Department,Lib.Member_DB[i].status,Lib.Member_DB[i].member_id,max);
			}
		}
	}
}


// Question No 6:

void Sort_Display(struct Library Lib, int member_data_size)
{
	int i,j,sorted,total_records=0;
	struct Member temp;
	sorted=0;
	
	while(Lib.Member_DB[total_records].Name[0]!='\0')
	{
		total_records++;
	}
	
	// using bubble sort to sort according to number of books borrowed
	
	for(i=0;(i<total_records-1)&&(sorted==0);i++)
	{
		sorted=1;
		for(j=0;j<total_records-1-i;j++)
		{
			// comparing books borrowed
			if(Lib.Member_DB[j].borrowed_no<Lib.Member_DB[j+1].borrowed_no)
			{
				// Swapping if number of books borrowed by successor is less
				
				temp=Lib.Member_DB[j];
				Lib.Member_DB[j]=Lib.Member_DB[j+1];
				Lib.Member_DB[j+1]=temp;
				sorted=0;
			}
			else if(Lib.Member_DB[j].borrowed_no==Lib.Member_DB[j+1].borrowed_no)
			{
				// If number of books borrowed are smme, then comparing status of successor
				
				if(string_cmp(Lib.Member_DB[j].status,Lib.Member_DB[j+1].status)==0)
				{
					// If number of books borrowed and status are same then comparing name of successor
					
					if(string_cmp(Lib.Member_DB[j].Name,Lib.Member_DB[j+1].Name)>0)
					{
						// If Successor name is alphabetically smaller, then swapping
						
						temp=Lib.Member_DB[j];
						Lib.Member_DB[j]=Lib.Member_DB[j+1];
						Lib.Member_DB[j+1]=temp;
						sorted=0;	
					}
				}
				
				// If status are not same, then priority to come early is given to Professor
				
				else if(Lib.Member_DB[j+1].status[0]=='P')
				{
					// If Successor if Professor, then swapping since status did not match
					
					temp=Lib.Member_DB[j];
					Lib.Member_DB[j]=Lib.Member_DB[i+1];
					Lib.Member_DB[j+1]=temp;
					sorted=0;	
				}
			}
		}
	}
}


main()
{
	struct Library Lib_DB;
	int status;
	Initialize_DB(Lib_DB,Book_DB_Size,Member_DB_Size);
	status=insert_book(Lib_DB,Book_DB_Size,Book_Name_Len,Name_Len,Sub_Len,4);
	status=new_member(Lib_DB,Member_DB_Size,Name_Len,Dep_Len,Stat_Len,4);
	Max_borrow(Lib_DB,Member_DB_Size);
	max_fine(Lib_DB,Member_DB_Size);
	Sort_Display(Lib_DB,Member_DB_Size);
}








