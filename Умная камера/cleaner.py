import os

UPLOAD_FOLDER = 'uploads'
if not os.path.exists(UPLOAD_FOLDER):
    os.makedirs(UPLOAD_FOLDER)

def cleanup_directory(directory_path, max_files=10):
    """
    Удаляет лишние файлы из указанной директории, оставляя только последние max_files файлов.
    
    :param directory_path: Путь к директории, которую нужно очистить.
    :param max_files: Максимальное количество файлов, которые нужно оставить.
    """
    if not os.path.exists(directory_path):
        print(f"Directory '{directory_path}' does not exist.")
        return

    # Получаем список файлов в директории
    files = [f for f in os.listdir(directory_path) if os.path.isfile(os.path.join(directory_path, f))]
    if len(files) <= max_files:
        print(f"No need to clean up. Only {len(files)} files found.")
        return

    # Сортируем файлы по времени изменения (самые старые первыми)
    files.sort(key=lambda x: os.path.getmtime(os.path.join(directory_path, x)))

    # Удаляем лишние файлы
    for old_file in files[:-max_files]:
        file_path = os.path.join(directory_path, old_file)
        try:
            os.remove(file_path)
            print(f"Deleted: {file_path}")
        except Exception as e:
            print(f"Error deleting {file_path}: {e}")

cleanup_directory(UPLOAD_FOLDER, max_files=10)