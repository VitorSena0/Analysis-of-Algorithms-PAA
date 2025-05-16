import re
from difflib import SequenceMatcher
from sklearn.feature_extraction.text import TfidfVectorizer

def tokenize_code(code):
    """
    Extrai tokens significativos do código-fonte, removendo espaços, 
    comentários e strings literais.
    """
    code = re.sub(r'//.*|/\*[\s\S]*?\*/', '', code)  # Remove comentários
    code = re.sub(r'"[^"\n]*"', '', code)  # Remove strings literais
    tokens = re.findall(r'\b\w+\b|[{}();,=+\-*/<>]', code)  # Captura identificadores, operadores, etc.
    return tokens

def count_code_features(code):
    """
    Conta elementos do código como número de funções, classes e variáveis.
    """
    functions = re.findall(r'\b(?:void|int|float|double|char|bool|string)\s+(\w+)\s*\(.*\)', code)
    variables = re.findall(r'\b(?:int|float|double|char|bool|string)\s+(\w+)', code)
    classes = re.findall(r'\b(?:class|struct)\s+(\w+)', code)

    return {
        "functions": len(set(functions)),
        "variables": len(set(variables)),
        "classes": len(set(classes))
    }

def lexical_similarity(code1, code2):
    """
    Calcula similaridade léxica baseada na frequência dos tokens (TF-IDF).
    """
    vectorizer = TfidfVectorizer()
    vectors = vectorizer.fit_transform([code1, code2])
    return (vectors * vectors.T).toarray()[0, 1] * 100  # Convertendo para porcentagem

def calcular_similaridade(arquivo1, arquivo2):
    # Ler os arquivos
    with open(arquivo1, "r", encoding="utf-8") as file1, open(arquivo2, "r", encoding="utf-8") as file2:
        file1_content = file1.read()
        file2_content = file2.read()

    # Comparação de tokens
    tokens_file1 = tokenize_code(file1_content)
    tokens_file2 = tokenize_code(file2_content)
    token_similarity = SequenceMatcher(None, tokens_file1, tokens_file2).ratio() * 100

    # Similaridade léxica (TF-IDF)
    tfidf_similarity = lexical_similarity(file1_content, file2_content)

    # Contagem de funções, variáveis e classes
    features_file1 = count_code_features(file1_content)
    features_file2 = count_code_features(file2_content)

    feature_comparison = {
        "functions_similarity": min(features_file1["functions"], features_file2["functions"]) / max(features_file1["functions"], features_file2["functions"]) * 100 if max(features_file1["functions"], features_file2["functions"]) > 0 else 0,
        "variables_similarity": min(features_file1["variables"], features_file2["variables"]) / max(features_file1["variables"], features_file2["variables"]) * 100 if max(features_file1["variables"], features_file2["variables"]) > 0 else 0,
        "classes_similarity": min(features_file1["classes"], features_file2["classes"]) / max(features_file1["classes"], features_file2["classes"]) * 100 if max(features_file1["classes"], features_file2["classes"]) > 0 else 0
    }

    return {
        "token_similarity": token_similarity,
        "tfidf_similarity": tfidf_similarity,
        "feature_comparison": feature_comparison
    }

# Caminhos dos arquivos (substitua pelos seus arquivos reais)
arquivo1 = "rafaelmachado_202300027623_transportadora.cpp"
arquivo2 = "transportadora.c"

# Executar a análise de similaridade
resultados = calcular_similaridade(arquivo1, arquivo2)

# Exibir resultados
print(f"🔍 Similaridade baseada em tokens: {resultados['token_similarity']:.2f}%")
print(f"📊 Similaridade léxica (TF-IDF): {resultados['tfidf_similarity']:.2f}%")
print(f"🔢 Similaridade de funções: {resultados['feature_comparison']['functions_similarity']:.2f}%")
print(f"🔠 Similaridade de variáveis: {resultados['feature_comparison']['variables_similarity']:.2f}%")
print(f"🏛️ Similaridade de classes: {resultados['feature_comparison']['classes_similarity']:.2f}%")
