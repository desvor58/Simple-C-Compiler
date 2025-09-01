#ifndef HASHMAP_H
#define HASHMAP_H

#include "types.h"
#include "crc32.h"

#define genhashmap(Ty)  \
typedef struct {  \
    char *key;  \
    Ty *val;  \
} hashmap_##Ty##_container_t;  \
genlist(hashmap_##Ty##_container_t)  \
typedef struct {  \
    list_hashmap_##Ty##_container_t_pair_t *arr[HASHMAP_BUCKIT_NUM];  \
    char *keys[MAX_HASHMAP_KEYS_BUF];  \
    size_t keys_top;  \
} hashmap_##Ty##_t;  \
hashmap_##Ty##_t *hashmap_##Ty##_create()  \
{  \
    hashmap_##Ty##_t *map = (hashmap_##Ty##_t*)malloc(sizeof(hashmap_##Ty##_t));  \
    for (size_t i = 0; i < MAX_HASHMAP_KEYS_BUF; i++) {  \
        map->keys[i] = malloc(sizeof(char) * MAX_IDENT_SIZE);  \
    }  \
    map->keys_top = 0;  \
    for (size_t i = 0; i < HASHMAP_BUCKIT_NUM; i++) {  \
        map->arr[i] = list_hashmap_##Ty##_container_t_create();  \
    }  \
    return map;  \
}  \
int hashmap_##Ty##_set(hashmap_##Ty##_t *map, char *key, Ty *val)  \
{  \
    if (map == 0 || strlen(key) == 0 || val == 0) {  \
        return 1;  \
    }  \
    strcpy(map->keys[map->keys_top++], key);  \
    u32 buckit = crc32(key, strlen(key)) % HASHMAP_BUCKIT_NUM;  \
    hashmap_##Ty##_container_t *new_container = (hashmap_##Ty##_container_t*)malloc(sizeof(hashmap_##Ty##_container_t));  \
    new_container->key = key;  \
    new_container->val = val;  \
    for (int i = 0; i < list_hashmap_##Ty##_container_t_size(map->arr[buckit]); i++) {  \
        if (!strcmp(list_hashmap_##Ty##_container_t_get(map->arr[buckit], i)->key, key)) {  \
            free(list_hashmap_##Ty##_container_t_get(map->arr[buckit], i));  \
            list_hashmap_##Ty##_container_t_set(map->arr[buckit], i, new_container);  \
            return 0;  \
        }  \
    }  \
    list_hashmap_##Ty##_container_t_add(map->arr[buckit], new_container);  \
    return 0;  \
}  \
Ty *hashmap_##Ty##_get(hashmap_##Ty##_t *map, char *key)  \
{  \
    if (map == 0 || strlen(key) == 0) {  \
        return 0;  \
    }  \
    u32 buckit = crc32(key, strlen(key)) % HASHMAP_BUCKIT_NUM;  \
    if (list_hashmap_##Ty##_container_t_size(map->arr[buckit]) == 0) {  \
        return 0;  \
    }  \
    for (int i = 0; i < list_hashmap_##Ty##_container_t_size(map->arr[buckit]); i++) {  \
        if (!strcmp(list_hashmap_##Ty##_container_t_get(map->arr[buckit], i)->key, key)) {  \
            return list_hashmap_##Ty##_container_t_get(map->arr[buckit], i)->val;  \
        }  \
    }  \
    return 0;  \
}  \
void hashmap_##Ty##_free(hashmap_##Ty##_t *map)  \
{  \
    for (size_t i = 0; i < HASHMAP_BUCKIT_NUM; i++) {  \
        for (size_t j = 0; j < list_hashmap_##Ty##_container_t_size(map->arr[i]); i++) {  \
            free(list_hashmap_##Ty##_container_t_get(map->arr[i], j));  \
        }  \
        list_hashmap_##Ty##_container_t_free(map->arr[i]);  \
    }  \
}

#endif