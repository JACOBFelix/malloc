/*
** moulinette.c
*/

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct	s_structure
{
  char		*(*get_head[4])();
  void		*sbrk_current;
  int		page_size;
  void		*ptr;
  int		nb_test_passe;
  FILE		*stream;
  int		note_finale;
  int		points_totals;
  int		inconnue3;
  int		nb_reussi;
}		t_structure;

char	*get_triste()	{ return (":("); }
char	*get_normal()	{ return (":|"); }
char	*get_heureux()	{ return (":)"); }
char	*get_coeur()	{ return ("<3"); }

int	test0(t_structure *s) {
  void	*ptr, *tmp;
  int	result;

  s->nb_test_passe += 1;
  s->points_totals += 3;
  fprintf(s->stream, "Test 0 : Testing lil malloc/free and big malloc/free");

  if ((ptr = malloc(0x2A)) == (void*)0)
    return (0);
  memset(ptr, 0, 0x2A);
  free(ptr);

  if ((ptr = malloc(0x163E00)) == (void*)0)
    return (0);
  memset(ptr, 0, 0x163E00);
  free(ptr);

  tmp = sbrk(0);
  if (s->sbrk_current == tmp)
    result = 2;
  else {
    if ((tmp - s->sbrk_current) % s->page_size)
      result = 1;
    else if ((tmp - s->sbrk_current) / s->page_size > 2)
      result = 2;
    else
      result = 3;
  }
  return (result);
}

int	test1(t_structure *s) {
  void	*ptr, *tmp;
  int	result;

  s->nb_test_passe += 1;
  s->points_totals += 3;

  fprintf(s->stream, "Test 1 : Testing very big malloc/free");
  if (!(ptr = malloc(0x500000)))
    return (0);

  if (sbrk(0) == s->sbrk_current) {
    free(ptr);
    fprintf(s->stream, "WARNING: Allocating memory without using sbrk() is forbidden.\n");
    return (0);
  }

  tmp = sbrk(0);
  if ((tmp - s->sbrk_current) % s->page_size)
    result = 1;
  else if ((tmp - s->sbrk_current) / s->page_size > 2)
    result = 2;
  else
    result = 3;

  free(ptr);
  return (result);
}

int	test2(t_structure *s) {
  int	result = 0;
  void	*tmp;

  s->nb_test_passe += 1;
  s->points_totals += 3;
  s->sbrk_current = sbrk(0);

  fprintf(s->stream, "Test 2 : Testing very lil malloc/free");
  s->ptr = malloc(2);

  if (s->ptr) {
    tmp = sbrk(0);
    if (s->sbrk_current == tmp)
      result = 3;
    else if ((tmp - s->sbrk_current) % s->page_size)
      result = 1;
    else
      result = 2;
  }
  return (result);
}

int	test3(t_structure *s) {
  int	result, i, j;
  void	*end;
  void	*tab[11];

  s->nb_test_passe += 1;
  s->points_totals += 3;
  fprintf(s->stream, "Test 3 : Testing pow(2) malloc's/free's");
  s->sbrk_current = sbrk(0);

  j = 0;
  i = 1;

  while (i < 2048) {
    tab[j] = malloc(i);
    if (tab[j] == NULL)
      return (0);
    j += 1;
    i *= 2;
  }

  j -= 1;
  while (j >= 0) {
    free(tab[j]);
    j -= 1;
  }

  end = sbrk(0);
  if ((end - s->sbrk_current) % s->page_size)
    result = 1;
  else if ((end - s->sbrk_current) / s->page_size > 2)
    result = 2;
  else
    result = 3;
  return (result);
}

int	test4(t_structure *s) {
  void	*p1, *p2, *p3, *p4, *ptr;
  int	result;

  s->nb_test_passe += 1;
  s->points_totals += 3;
  fprintf(s->stream, "Test 4 : Testing pagesize malloc's/free's");

  p1 = sbrk(0);
  ptr = malloc(3 * s->page_size);
  p2 = sbrk(0);
  free(ptr);
  p3 = sbrk(0);

  ptr = malloc(s->page_size);
  p4 = sbrk(0);
  free(ptr);

  result = 0;
  if (p1 != p2) {
    if (p2 > p3)
      result = 1;
    if ((p3 - p1) / s->page_size <= 2)
      result += 1;
    if (p4 == p3)
      result += 1;
  }
  return (result);
}

int	test5(t_structure *s) {
  int	i, j, k, result;
  void	*ptr[1000];
  void	*end;

  s->nb_test_passe += 1;
  s->points_totals += 3;
  fprintf(s->stream, "Test 5 : Testing hundred of pow(2) malloc's/free's");

  i = k = 0;
  while (i < 100) {
    j = 1;
    while (j < 1024) {
      if ((ptr[k] = malloc(j)) == (void*)0)
	return (0);
      j *= 2;
      k += 1;
    }
    i += 1;
  }

  k -= 1;
  while (k >= 0) {
    free(ptr[k]);
    k -= 1;
  }

  end = sbrk(0);
  if ((end - s->sbrk_current) % s->page_size)
    result = 1;
  else if ((end - s->sbrk_current) / s->page_size > 2)
    result = 2;
  else
    result = 3;
  return (result);
}

int	test6(t_structure *s) {
  void	*start, *start2;
  void	*ptr[1000];
  int	i, j, k, var, result;

  s->nb_test_passe += 1;
  s->points_totals += 3;
  fprintf(s->stream, "Test 6 : Testing alternate lil/big malloc's");

  start = sbrk(0);
  start2 = start;

  i = k = var = 0;
  while (i < 100) {
    j = 1;
    while (j < 1024) {
      if ((ptr[k] = malloc(j)) == (void*)0)
	return (0);
      j *= 2;
      k += 1;
    }

    if ((s->page_size * -2 + start2 <= (start + s->page_size * (i - 1)))
	&& (start + s->page_size * (i - 1)) <= start2)
      var += 1;
    start2 = sbrk(0);

    i += 1;
  }

  k -= 1;
  while (k >= 0) {
    free(ptr[k]);
    k -= 1;
  }

  start2 = sbrk(0);
  if ((start2 - s->sbrk_current) % s->page_size || var == 0)
    result = 1;
  else if ((start2 - s->sbrk_current) / s->page_size > 2 || (i - 1) != var)
    result = 2;
  else
    result = 3;
  return (result);
}

int	test7(t_structure *s) {
  void	*ptr, *ptr2, *ptr3, *ptr4, *ptr5, *sbrk_tmp, *unused;
  int	result, size;

  s->nb_test_passe += 1;
  s->points_totals += 3;
  fprintf(s->stream, "Test 7 : Testing realloc's");

  size = getpagesize() * 42;
  if ((ptr = realloc(0, 2)) == (void*)0)
    return (0);
  if ((ptr2 = realloc(ptr, size)) == (void*)0)
    return (0);

  ptr3 = memset(ptr2, 0, size);
  unused = realloc(ptr3, 0);	// Affectation pour eviter warning

  result = 1;
  ptr4 = realloc(0, getpagesize() + size);
  sbrk_tmp = sbrk(0);

  if ((ptr5 = realloc(ptr4, 0x1F4)) == (void*)0)
    return (0);

  if (ptr4 == ptr5)
    result = 2;
  if (sbrk(0) == sbrk_tmp)
    result += 1;
  unused = realloc(ptr5, 0);
  (void)unused;

  return (result);
}

int	test9(t_structure *s) {
  void	*tmp;
  int	result;

  s->nb_test_passe += 1;
  s->points_totals += 3;
  fprintf(s->stream, "Test 9 : Various Dumb Tests...");

  if (malloc(0) == (void*)0)
    return (0);
  /* if (malloc(0xFFFFFFFB))		Y avait ce test mais j'sais pas pourquoi */
  /*   return (0); */

  tmp = sbrk(0);
  if ((tmp - s->sbrk_current) % s->page_size)
    result = 1;
  else if ((tmp - s->sbrk_current) / s->page_size > 2)
    result = 2;
  else
    result = 3;
  return (result);
}

int	calcul_chelou(int result) {
  if ((((result & 0x7F) + 1) >> 1) <= 0) {
    if ((result & 0xFF00) >> 8 != 0x8B && result != -1)
      result = 2;
    else
      result = 0;
  }
  else
    result = 0;
  return (result);
}

int	test_exec0(t_structure *s) {
  int	result = 0;

  s->nb_test_passe += 1;
  s->points_totals += 2;
  fprintf(s->stream, "Test Exec 0 : Testing standard ls test's");

  if ((calcul_chelou(system("ls > /dev/null")))
      && (calcul_chelou(system("ls -l > /dev/null")))
      && (calcul_chelou(system("ls -l /home > /dev/null"))))
    result = 2;
  return (result);
}

int	test_exec1(t_structure *s) {
  s->nb_test_passe += 1;
  s->points_totals += 2;
  fprintf(s->stream, "Test Exec 1 : Testing ls -lRa /tmp");
  return (calcul_chelou(system("ls -lRa /tmp > /dev/null")));
}

int	test_exec2(t_structure *s) {
  s->nb_test_passe += 1;
  s->points_totals += 2;
  fprintf(s->stream, "Test Exec 2 : Testing ls -lRa /home");
  return (calcul_chelou(system("ls -lRa /home > /dev/null")));
}

int	test_exec3(t_structure *s) {
  s->nb_test_passe += 1;
  s->points_totals += 2;
  fprintf(s->stream, "Test Exec 3 : Testing ls -lRa /dev");
  return (calcul_chelou(system("ls -lRa /dev > /dev/null")));
}

int	test_exec4(t_structure *s) {
  int	result = 0;

  s->nb_test_passe += 1;
  s->points_totals += 2;
  fprintf(s->stream, "Test Exec 4 : Testing bc");
  system("echo \"42+42\nquit\" > /tmp/calc");
  result = calcul_chelou(system("bc < /tmp/calc > /dev/null"));
  system("rm -rf /tmp/calc");
  return (result);
}

void	afficher_smiley(int resultat_test, t_structure *s) {
  fprintf(s->stream, " => %s\n", s->get_head[resultat_test]());
}

void	afficher_result(t_structure *s) {
  fprintf(s->stream, "\nYou passed %d/%d tests.\n", s->nb_reussi, s->nb_test_passe);
  fprintf(s->stream, "Note Finale: %d/%d\t; Note ramener a 8 dans le binaire, sans compter les threads\n",
	  s->note_finale, s->points_totals);
}

int main() {
  int		(*test[])() = { test0, test1, test2, test3, test4, test5, test6, test7, test9,
  				test_exec0, test_exec1, test_exec2, test_exec3, test_exec4 };



  t_structure	s = { { get_triste, get_normal, get_heureux, get_coeur },
		      sbrk(0), getpagesize(), (void*)0, 0, fdopen(dup(2), "a+"), 0, 0, 0, 0 };

  int		i = 0;
  int		resultat_test;

  fprintf(s.stream,
	  "Legend... :\n:( => Trou d'balle\n:| => Tu pues\n:) => T'y es presque\n<3 => Pas mal\nBy Moi\n\n");
  close(2);

  while (i <= 13)
  {
    resultat_test = (test[i])(&s);
    if (resultat_test > 0)
      s.nb_reussi += 1;
    if (i == 3)
      free(s.ptr);
    s.note_finale += resultat_test;
    afficher_smiley(resultat_test, &s);
    i += 1;
  }
  afficher_result(&s);
  return (0);
}
