echo "----------------------------------"
echo "  TESTES DE ACESSO"
echo "----------------------------------"
echo ""

echo "Teste 1: acesso em arquivo legivel"
./webspace test_webspace/ readable_file.html
echo ""

echo "Teste 2: acesso em diretorio com index.html"
./webspace test_webspace/ /
echo ""

echo "Teste 3: acesso em diretorio com welcome.html"
./webspace test_webspace/ readable_dir/
echo ""

echo "Teste 8: acesso em caminho inexistente"
./webspace test_webspace/ index2.html
echo ""

echo "----------------------------------"
echo "  TESTES DE PERMISSAO"
echo "----------------------------------"
echo ""

echo "Teste 4: acesso em arquivo ilegivel"
./webspace test_webspace/ unreadable_file.html
echo ""

echo "Teste 5: acesso em diretorio sem permissao de execucao"
./webspace test_webspace/ unreadable_dir/
echo ""

echo "Teste 6: acesso em diretorio com index.html ilegivel"
./webspace test_webspace/ readable_dir2/
echo ""

echo "Teste 7: acesso em diretorio com welcome.html ilegivel"
./webspace test_webspace/ readable_dir3/
echo ""


