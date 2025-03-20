from flask import Flask, request, jsonify
import os
from datetime import datetime  # Импортируем модуль для работы с датой и временем

app = Flask(__name__, template_folder='templates')

# Создаем директорию uploads при старте сервера
if not os.path.exists('uploads'):
    os.makedirs('uploads')

@app.route('/upload', methods=['POST'])
def upload():
    # Проверяем, есть ли данные в теле запроса
    if not request.data:
        return 'No data received', 400

    # Получаем текущую дату и время
    current_time = datetime.now().strftime('%Y-%m-%d_%H-%M-%S')  # Формат: ГГГГ-ММ-ДД_ЧЧ-ММ-СС

    # Генерируем уникальное имя для файла с датой и временем
    filename = f"image_{current_time}.jpg"
    file_path = os.path.join('uploads', filename)

    # Сохраняем полученные данные в файл
    with open(file_path, 'wb') as f:
        f.write(request.data)

    return f'File saved to {file_path}', 200


if __name__ == "__main__":
    app.run(host='0.0.0.0', port=8000)