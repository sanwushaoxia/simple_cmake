#include <cstdio>
#include <cstdlib>

struct ListNode {
    int val;
    struct ListNode *next;
};

// 合并两个已排序的链表
struct ListNode* merge(struct ListNode *l1, struct ListNode *l2) {
    struct ListNode dummy = {0, NULL};
    struct ListNode *p = &dummy;

    while (l1 && l2) {
        if (l1->val < l2->val) {
            p->next = l1;
            l1 = l1->next;
        } else {
            p->next = l2;
            l2 = l2->next;
        }
        p = p->next;
    }

    p->next = (l1 ? l1 : l2);
    return dummy.next;
}

// 获取链表长度
int getLength(struct ListNode *head) {
    int len = 0;
    while (head) {
        ++len;
        head = head->next;
    }
    return len;
}

// 链表归并排序（自底向上）
struct ListNode* sortList(struct ListNode *head) {
    if (head == NULL) {
        return NULL;
    }
    if (head->next == NULL) {
        return head;
    }

    struct ListNode dummy = {0, head};
    int len = getLength(head);

    for (int size = 1; size < len; size *= 2) {
        struct ListNode *prev = &dummy;
        struct ListNode *curr = dummy.next;

        while (curr) {
            struct ListNode *left = curr;
            for (int i = 1; i < size && curr->next; ++i) {
                curr = curr->next;
            }

            /* 此时 curr 为 left 链最后一个 Node */
            struct ListNode *right = curr->next;
            curr->next = NULL; // 将 left 链和 right 链断开
            curr = right;

            if (right == NULL) {
                /* 此时 right 链不存在, 无需合并 left 链和 right 链 */
                prev->next = left;
                break;
            }

            for (int i = 1; i < size && curr->next; ++i) {
                curr = curr->next;
            }
            struct ListNode *next = curr->next;
            curr->next = NULL; // 将 right 链和剩下的链断开
            curr = next;

            prev->next = merge(left, right);
            /* 将 prev 移至 merge 链的末尾 */
            while (prev->next) {
                prev = prev->next;
            }
        }
    }

    return dummy.next;


    // if (head == NULL || head->next == NULL) {
    //     return head;
    // }
    // int len = getLength(head);
    // struct ListNode dummy = {0, head};

    // for (int size = 1; size < len; size *= 2) {
    //     struct ListNode *prev = &dummy;
    //     struct ListNode *curr = dummy.next;

    //     while (curr) {
    //         // 分割出第一段 [curr, curr + size)
    //         struct ListNode *left = curr;
    //         for (int i = 1; i < size && curr->next; ++i) {
    //             curr = curr->next;
    //         }

    //         // 分割出第二段 [curr + size, curr + size * 2)
    //         struct ListNode *right = curr->next;
    //         curr->next = NULL; // 断开 left 链与 right 链
    //         curr = right;

    //         // 如果没有 right 段，直接连接 left
    //         if (!right) {
    //             prev->next = left;
    //             break;
    //         }

    //         // 找到 right 段的结尾并断开
    //         for (int i = 1; i < size && curr && curr->next; ++i) {
    //             curr = curr->next;
    //         }
    //         struct ListNode *next = NULL;
    //         if (curr) {
    //             next = curr->next;
    //             curr->next = NULL; // 断开 right 链与后续链
    //         }

    //         // 合并 left 和 right
    //         struct ListNode *merged = merge(left, right);
    //         prev->next = merged;

    //         // 移动 prev 到 merged 末尾
    //         while (prev->next) {
    //             prev = prev->next;
    //         }

    //         curr = next;
    //     }
    // }

    // return dummy.next;
}

int list_sort() {
    // 构建链表: 4 -> 2 -> 1 -> 3
    struct ListNode *head = (struct ListNode *)malloc(sizeof(struct ListNode));
    head->val = 4;
    head->next = (struct ListNode *)malloc(sizeof(struct ListNode));
    head->next->val = 2;
    head->next->next = (struct ListNode *)malloc(sizeof(struct ListNode));
    head->next->next->val = 1;
    head->next->next->next = (struct ListNode *)malloc(sizeof(struct ListNode));
    head->next->next->next->val = 3;
    head->next->next->next->next = NULL;

    head = sortList(head);

    // 打印结果: 1 2 3 4
    for (struct ListNode *p = head; p; p = p->next) {
        printf("%d ", p->val);
    }
    printf("\n");

    // 释放内存（略）
    return 0;
}