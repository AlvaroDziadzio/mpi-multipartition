import os
import re
import matplotlib.pyplot as plt
from fpdf import FPDF

# Função para processar os arquivos e extrair as informações
def process_file(file_path, num_ops):
    try:
        with open(file_path, 'r') as file:
            content = file.read()

        # Regex para extrair o tempo de cada operação
        match = re.search(r"multiPartitionTime deltaT\(ns\): (\d+) ns for (\d+) ops", content)
        if match:
            delta_t_ns = int(match.group(1))

            # Convertendo tempo de ns para segundos
            total_time_s = delta_t_ns / 1e9

            # Calculando a vazão (milhões de elementos particionados por segundo)
            throughput = (8000000 * 10) / (total_time_s * 1e6)

            return total_time_s, throughput
        else:
            print(f"Formato inesperado no arquivo: {file_path}")
            return None, None
    except Exception as e:
        print(f"Erro ao processar {file_path}: {e}")
        return None, None

# Função para processar o diretório
def parse_directory(directory):
    dir_name = os.path.basename(directory)
    try:
        num_ops = int(dir_name.replace('k', '000'))
    except ValueError:
        print(f"Nome do diretório inválido: {dir_name}")
        return {}

    stats = {}
    for num_threads in range(1, 9):  # 1 a 8 threads
        file_name = f"saida{num_threads}.txt"
        file_path = os.path.join(directory, file_name)

        if os.path.exists(file_path):
            total_time, throughput = process_file(file_path, num_ops)
            if total_time is not None:
                stats[num_threads] = (total_time, throughput)
        else:
            print(f"Arquivo não encontrado: {file_path}")
    return stats

# Função para gerar gráficos
def plot_stats(stats, title, ylabel, filename):
    threads = list(stats.keys())
    values = [stat[0] if ylabel == "Tempo (s)" else stat[1] for stat in stats.values()]

    plt.figure()
    plt.plot(threads, values, marker='o')
    plt.title(title)
    plt.xlabel("Número de Threads")
    plt.ylabel(ylabel)
    plt.grid(True)
    plt.savefig(filename)
    plt.close()

def main():
    current_dir = os.getcwd()
    pdf = FPDF()
    pdf.set_auto_page_break(auto=True, margin=15)
    
    # Informações iniciais no PDF
    pdf.add_page()
    pdf.set_font("Arial", size=12)
    pdf.cell(200, 10, txt="Relatório de Resultados", ln=True, align="C")
    pdf.ln(10)
    pdf.set_font("Arial", size=10)
    pdf.multi_cell(0, 10, txt="Nomes: Victor Ribeiro Garcia (GRR20203954) e Álvaro R. S. Dziadzio (GRR20203913)")

    for sub_dir in ['1k', '100k']:
        directory_path = os.path.join(current_dir, sub_dir)
        if not os.path.exists(directory_path):
            print(f"Diretório não encontrado: {directory_path}")
            continue

        stats = parse_directory(directory_path)
        if not stats:
            continue

        # Gráficos
        time_graph = f"{sub_dir}_tempo.png"
        throughput_graph = f"{sub_dir}_vazao.png"

        plot_stats(stats, f"Tempo por Número de Threads ({sub_dir})", "Tempo (s)", time_graph)
        plot_stats(stats, f"Vazão por Número de Threads ({sub_dir})", "Vazão (milhões de elementos/s)", throughput_graph)

        # Adicionando ao PDF
        pdf.add_page()
        pdf.set_font("Arial", size=12)
        pdf.cell(200, 10, txt=f"Resultados para {sub_dir}", ln=True, align="C")
        pdf.ln(10)

        for num_threads, (total_time, throughput) in stats.items():
            pdf.set_font("Arial", size=10)
            pdf.cell(0, 10, txt=f"{num_threads} threads - Tempo: {total_time:.6f}s, Vazão: {throughput:.2f}M elem/s", ln=True)
        pdf.ln(5)

        pdf.image(time_graph, x=10, y=None, w=190)
        pdf.add_page()
        pdf.image(throughput_graph, x=10, y=None, w=190)

    # Salvando o PDF
    output_pdf = "resultados.pdf"
    pdf.output(output_pdf)
    print(f"PDF gerado: {output_pdf}")

if __name__ == "__main__":
    main()

