from keras.models import Sequential
from keras.layers import Dense
import pandas as pd
import numpy as np
from keras.utils import to_categorical
from sklearn.model_selection import train_test_split
from keras.callbacks import EarlyStopping
from keras.callbacks import ModelCheckpoint
from keras.models import load_model
from sklearn.preprocessing import normalize
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import confusion_matrix
from keras.utils import plot_model


#obejrzenie danych
dataset = pd.read_csv('heart.csv')
print(dataset.describe())
print(dataset.shape)

#sprawdzamy ilość brakujących wartości
dataset = dataset.replace({'?':np.NaN})
dataset['num'] = dataset['num'].replace({2:1, 3:1, 4:1})
print(dataset.head())

for col in dataset.columns.values:
    print(col)
    print(dataset.loc[dataset[col].isna()].shape)

#usuwamy wiersze z brakującymi wartościami, ponieważ jest ich mało   
dataset = dataset.dropna()
print(dataset.shape)

#one hot encoding
categorical_features = ['cp', 'restecg', 'slope', 'thal']
for cat in categorical_features:
    one_hot = pd.get_dummies(dataset[cat], prefix=cat)
    one_hot = one_hot.drop(one_hot.columns.values[0], axis = 1)
    dataset = dataset.drop(cat, axis = 1)
    dataset = dataset.join(one_hot)
    
print(dataset.head())
print(dataset.shape)

#podział zestawu danych na parametry i zmienne docelowe
predictors = dataset.drop('num', axis = 1)
predictors = normalize(predictors)
scaler = StandardScaler()
#predictors = scaler.fit_transform(predictors)
target = to_categorical(dataset['num'])
#podział na zestaw treningowy i zestaw do weryfikacji
X_train, X_test, y_train, y_test = train_test_split(predictors, target, test_size = 0.4, random_state=42, stratify = dataset['num'])
#podział na zestaw, wykorzystywany później do dobrania stanu modelu i na zestaw do ostatecznej weryfikacji
X_test1, X_test2, y_test1, y_test2 = train_test_split(X_test, y_test, test_size = 0.5,stratify = np.delete(y_test, 1, axis = 1), random_state=30)

#dopasowanie parametrów
n_nodes = 130 #liczba neuronów w każdej warstwie
n_layers = 3 #liczba warstw ukrytych

model = Sequential()#nowy model
model.add(Dense(n_nodes, activation = 'relu', input_shape = (dataset.shape[1] - 1,)))#pierwsza warstwa ukryta
for n in np.linspace(0,1, num = n_layers - 1):#dodajemy pozostałe warstwy
    model.add(Dense(n_nodes, activation = 'relu'))

model.add(Dense(2, activation = 'softmax'))#warstwa wyjścia
model.compile(optimizer = 'adam', loss = 'categorical_crossentropy', metrics = ['acc'])
stopper = EarlyStopping(patience=15, monitor = 'val_loss')#wczesne zatrzymanie
checkpoint = ModelCheckpoint('model.h5', monitor = 'val_acc', save_best_only=True)#wybór najlepszego stanu modelu pod względem precyzji w zakresie zestawu testowego
plot_model(model, to_file='heart_model.png')
model.fit(X_train, y_train, epochs=150, callbacks=[stopper, checkpoint], batch_size=15, validation_data=(X_test1, y_test1))#uczenie się modelu
model = load_model('model.h5')
print('[loss, accuracy]')
print('dane walidacyjne')
print(model.evaluate(X_test1, y_test1))
print('dane testowe')
print(model.evaluate(X_test2, y_test2))
y_pred = model.predict(X_test2)
y_pred = (np.delete(y_pred, 1, axis = 1) > 0.5) * 1
print(confusion_matrix(np.delete(y_test2, 1, axis=1), y_pred))#macierz dla oceny dokładności
print('cały zestaw danych')
print(model.evaluate(predictors, target))

