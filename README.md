
Toy Server
==========

Gabriela Surita

Projeto desenvolvido para a disciplina de EA072 - Laboratório de Software Básico

Instalação
----------

Com GCC, Flex e Yacc instalados, execute:

```
make build
```

Os binários gerados ficam no diretório `_build`.
Para executar o servidor com o conteúdo do diretório `html`
na porta `8000`, use o comando:

```
make serve
```

Testando
--------

Os testes são escritos em Python e dependem dos pacotes
requests e pytest. A maneira recomendada de instalá-los é
via pip e virtualenv. Se você nâo os possuí, utilize o
gerenciador de pacotes do seu sistema.

Em sistemas Ubuntu/Debian:

```
sudo apt-get install python-pip python-virtualenv
```

Em sistemas Fedora/RedHat/CentOS:

```
sudo dnf install python-pip python-virtualenv
```

Com os requisitos instalados, basta executar:

```
make tests
```

**Observação:** se o seu sistema possuí pip, mas não
virtualenv e você não tem privilégios de administrador
(caso dos laboratórios da FEEC), use o comando abaixo
para instalar o pacote virtualenv:

```
pip install --user virtualenv
```


Uso avançado
------------

É possível mudar a porta, diretório raiz, número de threads e localização
do arquivo de log do servidor fazendo:

```
./_build/servidor <porta> <webspace> <logfile> <threads>
```

Para limpar logs e
