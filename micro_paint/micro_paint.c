#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct	s_zone
{
	int	width;
	int	height;
	char	background;
}		t_zone;

typedef struct	s_shape
{
	char	type;
	float	x;
	float	y;
	float	width;
	float	height;
	char	color;
}		t_shape;

int	ft_strlen(char *str)
{
	int	i = 0;

	while (str[i])
		i++;
	return (i);
}

int	str_error(char *str)
{
	write(1, str, ft_strlen(str));
	return (1);
}

int	clear_all(FILE *file, char *drawing)
{
	fclose(file);
	if (drawing)
		free(drawing);
	return (1);
}

int	check_zone(t_zone *zone)
{
	return (zone->width > 0 && zone->height > 0 && zone->width <= 300 && zone->height <= 300);
}

int	check_shape(t_shape *shape)
{
	return (shape->width > 0.00000000 && shape->height > 0.00000000 && (shape->type == 'r' || shape->type == 'R'));
}

int	get_zone(FILE *file, t_zone *zone)
{
	int	scan_ret;

	if ((scan_ret = fscanf(file, "%d %d %c\n", &zone->width, &zone->height, &zone->background)) != 3)
		return (0);
	if (!check_zone(zone))
		return (0);
	return (1);
}

char	*paint_background(t_zone *zone)
{
	char	*drawing;
	int	i = 0;

	if (!(drawing = malloc(sizeof(char) * ((zone->width * zone->height) + 1))))
		return (NULL);
	while (i < (zone->width * zone->height))
	{
		drawing[i] = zone->background;
		i++;
	}
	drawing[i] = '\0';
	return (drawing);
}
/*
int	in_circle(float x, float y, t_shape *shape)
{
	float	distance;

	distance = sqrtf(powf(x - shape->x, 2.) + powf(y - shape->y, 2.));
	if (distance <= shape->radius)
	{
		if (shape->radius - distance < 1.00000000)
			return (2);
		return (1);
	}
	return (0);
}
*/
int	in_rectangle(float x, float y, t_shape *shape)
{
	if (x < shape->x || y < shape->y || x > shape->x + shape->width || y > shape->y + shape->height)
		return (0);
	if (x - shape->x < 1.00000000 || y - shape->y < 1.00000000 || shape->x + shape->width - x < 1.00000000 || shape->y + shape->height - y < 1.00000000)
		return (2);
	return (1);
}

void	draw_shape(char **drawing, t_zone *zone, t_shape *shape)
{
	int	i;
	int	j;
	int	rect;

	i = 0;
	while (i < zone->height)
	{
		j = 0;
		while (j < zone->width)
		{
			rect = in_rectangle(j, i, shape);
			if ((shape->type == 'r' && rect == 2) || (shape->type == 'R' && rect))
				(*drawing)[(i * zone->width) + j] = shape->color;
			j++;
		}
		i++;
	}	
}

int	draw_shapes(FILE *file, char **drawing, t_zone *zone)
{
	t_shape	tmp;
	int	scan_ret;

	while ((scan_ret = fscanf(file, "%c %f %f %f %f %c\n", &tmp.type, &tmp.x, &tmp.y, &tmp.width, &tmp.height, &tmp.color)) == 6)
	{
		if (!check_shape(&tmp))
			return (0);
		draw_shape(drawing, zone, &tmp);
	}
	if (scan_ret != -1)
		return (0);
	return (1);
}

void	draw_drawing(char *drawing, t_zone *zone)
{
	int	i = 0;

	while (i < zone->height)
	{
		write(1, drawing + (i * zone->width), zone->width);
		write(1, "\n", 1);
		i++;
	}
}

int	main(int ac, char **av)
{
	t_zone	zone;
	char	*drawing;
	FILE	*file;

	zone.width = 0;
	zone.height = 0;
	zone.background = 0;
	drawing = NULL;
	if (ac != 2)
		return (str_error("Error: argument\n"));
	if (!(file = fopen(av[1], "r")))
		return (str_error("Error: Operation file corrupted\n"));
	if (!get_zone(file, &zone))
		return (clear_all(file, NULL) && str_error("Error: Operation file corrupted\n"));
	if (!(drawing = paint_background(&zone)))
		return (clear_all(file, NULL) && str_error("Error: Operation file corrupted\n"));
	if (!draw_shapes(file, &drawing, &zone))
		return (clear_all(file, drawing) && str_error("Error: Operation file corrupted\n"));
	draw_drawing(drawing, &zone);
	clear_all(file, drawing);
	return (0);
}
