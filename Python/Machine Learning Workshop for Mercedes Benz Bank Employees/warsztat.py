import pandas as pd 
import numpy as np 
from sklearn.preprocessing import normalize
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
from keras.layers import concatenate
from keras.layers import Embedding
from keras.layers import Dense
from keras.layers import Reshape
from keras.layers import Input
from keras.models import Model
from keras.callbacks import ModelCheckpoint, ProgbarLogger
from keras.callbacks import EarlyStopping
from keras.utils import plot_model

#Wczytywanie danych
dataSet = pd.read_csv('modified4.csv',  index_col=0)
print(dataSet.head())
print(np.mean(dataSet['Price'].values))



#Przygotowanie danych (podział, normalizacja)
pred_train, pred_test, Y_train, Y_test = train_test_split(dataSet[['Year', 'Mileage', 'Name']], dataSet['Price'], test_size = 0.2, random_state = 42, stratify = dataSet['Name'])


X_train = pred_train[['Year', 'Mileage']]
X_test = pred_test[['Year', 'Mileage']]

print(Y_train)
X_train = normalize(X_train)
X_test = normalize(X_test)

print(Y_train)

print(X_train)
cat_X_train = pred_train['Name']
cat_X_test = pred_test['Name']

em_size = 50
unique = dataSet['Name'].max() + 1
input1 = Input(shape = (1,), name = 'car_models')
embed = Embedding(unique, em_size, input_length = 1, name='coding_car_models')(input1)
embed = Reshape(target_shape = (em_size,))(embed)

input2 = Input(shape = (2,), name = 'year_and_mileage')

n = 70 #liczba neuronów
m = 7  #liczba warstw

p = concatenate([embed, input2])
for i in range(m):
    p = Dense(n, activation = 'relu')(p)

print(p)

output = Dense(1, activation = 'linear', name = 'used_car_price')(p)

model = Model(inputs = [input1, input2], outputs = [output])
print(model)

#kompilowanie
model.compile(optimizer = 'adam', loss = 'mean_absolute_percentage_error')
plot_model(model, to_file='model.png', show_shapes = True)
stop = EarlyStopping(monitor = 'val_loss', patience = 5)
save = ModelCheckpoint(filepath = 'best_model.h5', monitor = 'val_loss', save_best_only=True)
model.fit([cat_X_train, X_train], Y_train, callbacks=[stop, save], epochs=50, batch_size=500, validation_split=0.2)
model.load_weights('best_model.h5')
loss = model.evaluate([cat_X_test, X_test], Y_test)

print(loss)

