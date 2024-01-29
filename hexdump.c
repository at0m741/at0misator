/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hexdump.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouzali <ltouzali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 16:28:31 by ltouzali          #+#    #+#             */
/*   Updated: 2024/01/29 19:18:36 by ltouzali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"

void    printHex(unsigned char *data, size_t size)
{
    unsigned char *bytes = data;
    size_t i = 0;

    while (i < size)
    {
        printf("0x%016X ", (unsigned int)i);
        size_t j = 0;
        while (j < 16 && i + j < size)
        {
            printf ("%02X ", bytes[i + j]);
            j++;
        }
        while (j < 16)
        {
            printf("    ");
            j++;
        }
        printf("| ");
        j = 0;
        while (j < 16 && i + j < size)
        {
            if (bytes[j + i] >= 32 && bytes[i + j] <= 126)
            {
                printf("%c", bytes[i + j]);
            }
            else
            {
                printf(".");
            }
            j++;
        }
        printf("\n");
        i += 16;
    }
}