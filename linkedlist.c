#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

struct node{
	int data;
	struct node* next;
};

struct node* head = NULL;

int thread_count;
pthread_rwlock_t rwlock;
pthread_mutex_t mutex;
void Member(int value);
int Insert(int value);
void Delete(int value);
void printList( struct node* head_p );

int member_count=0;
int insert_count=0;
int delete_count=0;

float mMember = 0.25;
float mInsert = 0.25;
float mDelete = 0.25;

void* Thread_work(void* rank);

int a;
int n;

int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;

	pthread_rwlock_init(&rwlock, NULL);

	printf("Enter the number of tasks \n");
	scanf("%d" , &a);

	printf("Enter the number of threads : \n");
	scanf("%d" , &thread_count);

	printf("How many random sorted numbers? :\n");
	scanf("%d" , &n);
	thread_handles = malloc(thread_count*sizeof(pthread_t));

	int i=0;

	for(;i<n;i++){
		int r = (rand() % (100 - 10 + 1)) + 10;
		if(!Insert(r)){
			i--;
		}
	}

	printf("Linked List before any operation : ");
	printList(head);
	for(thread = 0; thread < thread_count; thread++) {
			pthread_create(&thread_handles[thread], NULL, Thread_work,
						   (void*) thread);
	}
	for(thread = 0; thread < thread_count; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}

	printf("Linked List after the operation : ");
	printList(head);
	printf("Member operation is performed : %d \nInsert operation is performed : %d \nDelete operation is performed : %d",member_count,insert_count,delete_count);
	pthread_rwlock_destroy(&rwlock);
	pthread_mutex_destroy(&mutex);
	free(thread_handles);

	return 0;
}


void *Thread_work(void* rank) {
	long my_rank = (long) rank;
	int i , val;
	int my_member=0;
	int my_insert=0;
	int my_delete=0;

	int ops_per_thread = a/thread_count;
	for (i = 0; i < ops_per_thread; i++) {
		float operation_choice = (rand()%10000/10000.0);
		val = (rand() % (200 - 20 + 1)) + 20;

		if (operation_choice < mMember) {
			pthread_rwlock_rdlock(&rwlock);
			printf("Th %ld > " , my_rank);
			Member(val);
			pthread_rwlock_unlock(&rwlock);
			my_member++;
		}
		else if (operation_choice < mMember + mInsert) {
			pthread_rwlock_wrlock(&rwlock);
			printf("Th %ld > " , my_rank);
			Insert(val);
			printf("Inserted %d successfully! \n" , val);
			printf("Linked list after insertion is :");
			printList(head);
			pthread_rwlock_unlock(&rwlock);
			my_insert++;
		}
		else {
			pthread_rwlock_wrlock(&rwlock);
			printf("Th %ld > " , my_rank);
			Delete(val);
			pthread_rwlock_unlock(&rwlock);
			my_delete++;
		}
	}
	pthread_mutex_lock(&mutex);
	member_count += my_member;
	insert_count += my_insert;
	delete_count += my_delete;
	pthread_mutex_unlock(&mutex);
	return NULL;
}

void Member(int value) {
    struct node* temp;

    temp = head;
    while (temp != NULL && temp->data < value)
        temp = temp->next;

    if (temp == NULL || temp->data > value) {
    	printf("%d is not a member in linked-list\n", value);
    } else {
    	printf("%d is a member in linked-list\n", value);
    }
}

int Insert(int value) {
    struct node* current = head;
    struct node* pred = NULL;
    struct node* temp;
    int return_value = 1;

    while (current != NULL && current->data < value) {
        pred = current;
        current = current->next;
    }

    if (current == NULL || current->data > value) {
        temp = malloc(sizeof(struct node));
        temp->data = value;
        temp->next = current;
        if (pred == NULL)
            head = temp;
        else
            pred->next = temp;
    }else{
    	return_value = 0;
    }
    return return_value;
}

void Delete(int value) {
   struct node* current = head;
   struct node* pred = NULL;

   while (current != NULL && current->data < value) {
      pred = current;
      current = current->next;
   }

   if (current != NULL && current->data == value) {
      if (pred == NULL) {
         head = current->next;
         free(current);
         printf("Deleted %d successfully! \n", value);
         printf("Linked list after deletion is :");
         printList(head);
      } else {
         pred->next = current->next;
         free(current);
         printf("Deleted %d successfully! \n", value);
         printf("Linked list after deletion is :");
         printList(head);
      }
   }
   else{
	   printf("%d is not in the linked-list\n", value);
   }
}

void printList( struct node* head_p )
{
    struct node* curr_p = head_p;

    while(curr_p != NULL)
    {
        printf("%d ",curr_p->data);
        curr_p = curr_p->next;
    }
    printf("\n");
}
