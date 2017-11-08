typedef struct Node
{
	struct Node *next;
	struct Node *prev;
	struct process *proc;
	int id;
	int size;
} Node;

void addToEnd(Node *head, struct process* proc, int id);
void addToEmpty(Node *head, struct process* proc, int id);
Node pop(Node *head);
