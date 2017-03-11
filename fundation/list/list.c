/****************************************************************************
 * halo -- Open HAL for embedded systems                                    *
 *                                                                          *
 * MIT License:                                                             *
 * Copyright (c) 2017 SimonQian                                             *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 ****************************************************************************/

#include "halo.h"

int list_is_in(struct list_t *head, struct list_t *node)
{
	while (head != (struct list_t *)0)
	{
		if (head == node)
		{
			return 1;
		}
		head = head->next;
	}
	return 0;
}

int list_remove(struct list_t **head, struct list_t *node)
{
	if (!list_is_in(*head, node))
	{
		return -1;
	}

	if (*head == node)
	{
		*head = node->next;
		return 0;
	}
	while (*head != (struct list_t *)0)
	{
		if ((*head)->next == node)
		{
			(*head)->next = node->next;
			break;
		}
		*head = (*head)->next;
	}
	return 0;
}

void list_append(struct list_t *head, struct list_t *new_node)
{
	struct list_t *next;

	next = head;
	while (next->next != NULL)
		next = next->next;

	next->next = new_node;
	new_node->next = NULL;
}

void list_delete_next(struct list_t *head)
{
	struct list_t *next;

	next = head->next;
	if (next->next)
		head->next = next->next;
	else
		head->next = NULL;
}
