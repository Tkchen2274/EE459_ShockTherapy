from flask import Flask, render_template, request, redirect, url_for, flash, jsonify
from werkzeug.utils import secure_filename
import os

app = Flask(__name__)
app.secret_key = 'your_secret_key'

UPLOAD_FOLDER = 'static/uploads'
INTRUDE_FOLDER = 'static/intrude'
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
app.config['DOORBELL_FOLDER'] = 'static/doorbell'
ALLOWED_EXTENSIONS = {'png', 'jpg', 'jpeg'}
target_ip = '127.0.0.1'

pending_commands = {}

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

@app.route('/')
def upload_file_form():
    # List all files in the uploads directory
    image_files = [os.path.join(app.config['UPLOAD_FOLDER'], file) for file in os.listdir(app.config['UPLOAD_FOLDER']) if file.endswith(tuple(['.jpg', '.png', '.gif', '.jpeg']))]
    return render_template('upload.html', images=image_files)


@app.route('/upload', methods=['POST'])
def upload_file():
    if 'file' not in request.files:
        flash('No file part')
        return redirect(request.url)
    file = request.files['file']
    if file.filename == '':
        flash('No selected file')
        return redirect(request.url)
    filename = request.form['filename']  # Extract the filename from the form data
    if filename == '':
        flash('No file name provided')
        return redirect(request.url)

    
    if file and allowed_file(file.filename):
        '''
        filename = secure_filename(file.filename)
        filepath = os.path.join(app.config['UPLOAD_FOLDER'], filename)
        file.save(filepath)
        '''

        filename = secure_filename(filename)  # Sanitize the filename
        file_ext = os.path.splitext(file.filename)[1]  # Get the file extension
        filename_with_ext = filename + file_ext  # Combine filename and extension

        filepath = os.path.join(app.config['UPLOAD_FOLDER'], filename_with_ext)
        file.save(filepath)

        
        # Update pending_commands
        if target_ip not in pending_commands:
            pending_commands[target_ip] = []

        command_with_pass_and_pic = {
            "name": 'new_face',
            "picture": filepath  # Save the path of the uploaded picture
        }
        pending_commands[target_ip].append(command_with_pass_and_pic)
 
        # file.save(os.path.join(app.config['UPLOAD_FOLDER'], filename))
        flash('File uploaded successfully')
        return redirect(url_for('upload_file_form'))
    else:
        flash('Allowed file types are .png, .jpg, .jpeg')
        return redirect(request.url)

@app.route('/handle_selection', methods=['POST'])
def handle_selection():
    # Retrieve a list of items that were selected.
    target_ip = '127.0.0.1'
 
    selected_items = request.form.getlist('items')
    # Do something with the selected items (e.g., process them, save to database)
    # for item in selected_items:
    #     print(item)  # Print each selected item 
    if selected_items:  # Check if the list is not empty
        if 'fingerprint' in selected_items:  # Check if 'fingerprint' is in the list
            print('finger')
            if target_ip not in pending_commands:
                pending_commands[target_ip] = []

    # store both the command name and the password as a dictionary
            command_fingerprint = {"name": 'addfingerprint'}
            pending_commands[target_ip].append(command_fingerprint)
       

    return redirect(url_for('upload_file_form'))
 

@app.route('/change_pass', methods=['POST'])
def change_pass():
    # data = request.json

    # password = data.get('password')
    password = password = request.form['password']
    # target_ip = data.get('target_ip')
    # command_name = data.get('command')
    command_name = 'pass_change'

    if not target_ip or not command_name or not password:
        return 'Missing target_ip, command, or password', 400

    if target_ip not in pending_commands:
        pending_commands[target_ip] = []

    # store both the command name and the password as a dictionary
    command_with_pass = {"name": command_name, "password": password}
    pending_commands[target_ip].append(command_with_pass)

    return redirect(url_for('upload_file_form'))
 

@app.route('/check-commands')
def check_commands():
    client_ip = request.remote_addr

    if client_ip in pending_commands and pending_commands[client_ip]:
        commands = pending_commands[client_ip]
        pending_commands[client_ip] = []
        return jsonify(commands)
    else:
        return 'No commands', 200

@app.route('/doorbell/', methods=['POST'])
def door_bell():
    if 'file' not in request.files:
        return jsonify({"error": "No file part"}), 400

    file = request.files['file']
    if file.filename == '':
        return jsonify({"error": "No selected file"}), 400

    if file and file.filename.endswith('.png'):
        filename = file.filename
        filepath = os.path.join(app.config['DOORBELL_FOLDER'], filename)
        file.save(filepath)
        
        return jsonify({"message": "File uploaded successfully", "filepath": filepath}), 200
    else:
        return jsonify({"error": "Invalid file type, only PNG files are allowed"}), 400
# curl -X POST -F "file=@./test2.png" http://127.0.0.1:5000/doorbell/
# command to use


if __name__ == '__main__':
    app.run(debug=True)

