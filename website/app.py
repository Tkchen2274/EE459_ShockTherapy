from flask import Flask, render_template, request
import os

app = Flask(__name__)

# This is the path to the directory where you want to save the uploaded files.
# Make sure this directory exists on your server.
UPLOAD_FOLDER = '/home/tyler/ee459/uploads'
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER

@app.route('/')
def upload_file_form():
    return render_template('upload.html')

@app.route('/upload', methods=['POST'])
def upload_file():
    if 'file' not in request.files:
        return 'No file part'
    file = request.files['file']
    if file.filename == '':
        return 'No selected file'
    if file:
        filename = file.filename
        file.save(os.path.join(app.config['UPLOAD_FOLDER'], filename))
        # return 'File uploaded successfully'
        return render_template('upload.html')

if __name__ == '__main__':
    app.run(debug=True)

