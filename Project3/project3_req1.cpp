/******************************************************************************************
 * Universidade de Brasília – UnB
 * Instituto de Ciências Exatas – IE
 * Departamento de Ciência da Computação – CIC
 * Visao Computacional – Professor Fabio Vidal
 *
 * Projeto 3 – Objetivo
 * Esta atividade tem como objetivo principal a exploração e desenvolvimento de algoritmos para extração de mapas de
 * profundidade a partir de pares estéreo de imagens.
 *
 * Procedimentos
 * Requisito 1 - Assumindo os 2(dois) pares de imagens capturadas por uma câmera estéreo calibrada e retificada
 * (link no item da atividade no moodle da disciplina), onde os índices L e R indicam as imagens da esquerda e direita
 * respectivamente, assumindo que estas imagens estejam perfeitamente alinhadas em paralelo, com uma distância focal
 * definida f = 25mm (medida hipotética) e baseline igual 120mm (também hipotética),
 * e que o as coordenadas do mundo X, Y são coincidentes com as coordenadas da imagem x, y em seu respectivo lado
 * (direito ou esquerdo) realize as seguintes tarefas:
 *
 * :1:
 * Desenvolva um algoritmo, usando OpenCV que para cada par de pontos x,y na imagem do lado esquerdo, encontre o ponto
 * respectivo par de pontos x,y na imagem da direita. Utilize a ideia básica do algoritmo apresentado em sala de aula,
 * onde a função de comparação seja a função SAD - http://en.wikipedia.org/wiki/Sum_of_absolute_differences,
 * onde a dimensão da janela para template W seja definida pelo usuário no momento do início do cálculo.
 *
 * Observe que não haverá correlação para todos os pontos das imagens, desta feita, para os casos onde o ponto da
 * esquerda não tenha correspondência com a da imagem da direita, realize uma organização de forma que ele seja
 * identificado e possa ser descartado nos cálculos futuros da profundidade, no próximo requisito.
 *
 * Recomendo a utilização em uma estrutura de dados (ex.: vetor/matriz) para armazenar a posição do ponto na imagem da
 * esquerda e na imagem da direita para auxiliar no processo de armazenagem de dados e evitar "estouros" de memória.
 *
 * :2:
 * Para todos os pontos que existem correspondência entre as imagens da direita e da esquerda, assumindo as condições
 * anteriormente expostas, calcule para cada um dos pontos os valores para X, Y e Z do mundo, seguindo as equações
 * apresentadas em sala de aula (slide 14).
 *
 * :3:
 * Para todos os pontos calculados, crie uma imagem resultante (obs.: provavelmente de dimensões diferentes do que as
 * imagens originais) com as informações de profundidade obtidas (valores de Z), normalizando os valores mínimos e
 * máximos obtidos entre 0 e 255, salvando a esta imagem de profundidade em um arquivo à parte e armazenado com o nome
 * da imagem de origem, acrescentando ao final do nome a palavra "depthimage" ao final, para identificação
 * após à execução do código.
 *
 * Nome: Jessé Barreto de Barros
 * Matrícula: 17/0067033
 ******************************************************************************************
*/

#include "project3.h"

int main(int argc, char** argv)
{

    return 0;
}



