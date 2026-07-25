#include <my/List.h>
#include <stdlib.h>

Node* Node::newNode(Element data) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node) {
        node->prev = nullptr;
        node->next = nullptr;
        node->data = data;
    }
    return node;
}

void Node::insert(Link pos, Link node) {
    if (!pos || !node) return;
    node->next = pos;
    node->prev = pos->prev;
    if (pos->prev) {
        pos->prev->next = node;
    }
    pos->prev = node;
}

void Node::disconnect() {
    if (prev) {
        prev->next = next;
    }
    if (next) {
        next->prev = prev;
    }
    prev = nullptr;
    next = nullptr;
}

List* List::newList() {
    List* list = (List*)malloc(sizeof(List));
    if (list) {
        list->head = Node::newNode(nullptr);
        list->tail = Node::newNode(nullptr);
        if (list->head && list->tail) {
            list->head->next = list->tail;
            list->tail->prev = list->head;
        }
        list->usage = 0;
    }
    return list;
}

void List::dispose() {
    Link curr = head;
    while (curr) {
        Link next = curr->next;
        free(curr);
        curr = next;
    }
    free(this);
}

void List::push_back(Element data) {
    Node* node = Node::newNode(data);
    if (node) {
        node->insert(tail, node);
        usage++;
    }
}

void List::insert(Element data) {
    push_back(data);
}

Link List::begin() {
    return head ? head->next : nullptr;
}

Link List::end() {
    return tail;
}

void List::erase(Link pos) {
    if (!pos || pos == head || pos == tail) return;
    pos->disconnect();
    free(pos);
    if (usage > 0) usage--;
}

bool List::find(Element data, int (*comp)(Element val)) {
    Link curr = begin();
    while (curr && curr != end()) {
        if (comp) {
            if (comp(curr->data) == 0) return true;
        } else {
            if (curr->data == data) return true;
        }
        curr = curr->next;
    }
    return false;
}
