/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   easyfind.tpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axelpeti <axelpeti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 14:08:34 by axelpeti          #+#    #+#             */
/*   Updated: 2026/01/23 09:40:39 by axelpeti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

template<typename T>
void easyfind(T& tab, int to_find)
{
    if (std::find(tab.begin(), tab.end(), to_find) == tab.end())
        throw std::out_of_range("occurence not find");
    else
        std::cout << "occurence find" << std::endl;
    return;
}

