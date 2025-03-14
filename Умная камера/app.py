from flask import Flask, request, jsonify
import os

app = Flask(__name__, template_folder='templates')

# Создаем директорию uploads при старте сервера
if not os.path.exists('uploads'):
    os.makedirs('uploads')

@app.route('/upload', methods=['POST'])
def upload():
    # Проверяем, есть ли данные в теле запроса
    if not request.data:
        return 'No data received', 400

    # Генерируем уникальное имя для файла
    file_path = os.path.join('uploads', 'image.jpg')

    # Сохраняем полученные данные в файл
    with open(file_path, 'wb') as f:
        f.write(request.data)

    return f'File saved to {file_path}', 200

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=8000)