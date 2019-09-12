<template>
  <div class="home">
    <header>
      <img src="../assets/logo-header.png">
      <p class="login-info">
        <i class="fas fa-user"></i>
        {{ user.displayName }}
      </p>
      <div>
        <button class="logout-button" @click="signOut">ログアウト</button>
      </div>
    </header>
    <div class="main" v-if="hide_qr">
      <div class="input-form">
        <div class="tapper">
          <input type="text" v-model="tapper_id" placeholder="タッパーID" />
          <i class="fas fa-box" aria-hidden="true"></i>
          <div class="qr-reader">
            <input
              type="image"
              alt="qr"
              src="https://dab1nmslvvntp.cloudfront.net/wp-content/uploads/2017/07/1499401426qr_icon.svg"
              @click="qr_page"
              width="25"
              height="25"
            />
          </div>
        </div>
      </div>

      <div class="input-form">
        <input type="text" v-model="food_name" placeholder="食材名" />
        <i class="fas fa-utensils" aria-hidden="true"></i>
      </div>

      <div class="input-form">
        <div class="exp-date">
          <input type="text" v-model="exp_date" placeholder="消費期限(yyyy/mm/dd)" />
          <i class="far fa-calendar-alt" aria-hidden="true"></i>
          <div class="add-button">
            <input
              type="image"
              alt="add"
              @click="addFoods"
              src="https://i.imgur.com/PIrzJwO.png"
              width="30"
              height="30"
            />
          </div>
        </div>
      </div>

      <li class="foods-list" v-for="item in list" :key="item.tapper_id" v-show="item.is_active">
        <p>タッパーID:{{ item.tapper_id }}</p>
        <p>食材名:{{ item.name }}</p>
        <p>温度:{{ item.temperature }}</p>
        <p>消費期限:{{ item.exp_date }}</p>
        <input
          type="image"
          class="delete-button"
          alt="delete"
          @click="deleteFoods(item.tapper_id)"
          src="https://i.imgur.com/sntj3EF.png"
          width="30"
          height="30"
        />
      </li>
    </div>
    <div class="qr" v-if="!hide_qr">
      <qrcode-stream @decode="onDecode" @init="onInit" />
      {{ error }}
      {{ result }}
    </div>
  </div>
</template>


<script>
import firebase from "firebase";
import { QrcodeStream } from "vue-qrcode-reader";

export default {
  name: "home",
  components: {
    QrcodeStream
  },
  data() {
    return {
      user: {}, // firebase ユーザ情報
      exp_date: "",
      tapper_id: null,
      list: [],
      food_name: "",
      result: "",
      error: "",
      hide_qr: true
    };
  },

  methods: {
    onDecode(result) {
      this.tapper_id = result;
      this.hide_qr = true;
    },

    async onInit(promise) {
      try {
        await promise;
      } catch (error) {
        if (error.name === "NotAllowedError") {
          this.error = "You need to grant camera access permisson";
        } else if (error.name === "NotFoundError") {
          this.error = "No camera on this device";
        } else if (error.name === "NotSupportedError") {
          this.error = "Secure context required (HTTPS, localhost)";
        } else if (error.name === "NotReadableError") {
          this.error = "Is the camera already in use?";
        } else if (error.name === "OverconstrainedError") {
          this.error = "Installed cameras are not suitable";
        } else if (error.name === "StreamApiNotSupportedError") {
          this.error = "Stream API is not supported in this browser";
        }
      }
    },

    // QR読取り
    qr_page() {
      this.hide_qr = false;
    },

    // ログアウト
    signOut() {
      firebase.auth().signOut();
      this.$router.push("/signin");
    },

    // firestore cloud に登録
    addFoods() {
      if (this.tapper_id != null && this.exp_date != "" && this.food_name != ""){
        firebase
          .firestore()
          .collection("foods")
          .doc(String(this.tapper_id))
          .set(
            {
              tapper_id: this.tapper_id,
              exp_date: this.exp_date,
              is_active: true,
              name: this.food_name
            },
            { merge: true }
          );
      }
      else {
        alert("入力に不備があります．")
      }

      this.tapper_id = null;
      this.exp_date = "";
      this.food_name = "";
    },

    // is_active -> falseにしてリスト非表示
    deleteFoods(id) {
      firebase
        .firestore()
        .collection("foods")
        .doc(String(id))
        .set(
          {
            exp_date: "",
            name: "",
            is_active: false
          },
          { merge: true }
        );
    }
  },

  created: function() {
    const user = firebase.auth().currentUser;

    if (user) {
      this.user = user;
    } else {
      this.$router.push("/signin");
    }

    // firestore からデータを取得
    firebase
      .firestore()
      .collection("foods")
      .onSnapshot(ss => {
        let buff = [];
        ss.forEach(doc => buff.push(doc.data()));
        this.list = buff;
      });
  }
};
</script>

<style scoped>
.foods-list {
  border-radius: 15px;
  width: 300px;
  text-align: center;
  background-color: aquamarine;
  margin: 20px;
  display: inline-block;
}

.input-form {
  position: relative;
  width: 10%;
  margin-left: 20px;
}

.input-form input[type="text"] {
  font: 15px/24px sans-serif;
  box-sizing: border-box;
  margin: 8px 0;
  padding: 0.3em;
  transition: 0.3s;
  border: 1px solid #1b2538;
  border-radius: 4px;
  outline: none;
}
.input-form input[type="text"]:focus {
  border-color: #da3c41;
}
.input-form input[type="text"] {
  padding-left: 40px;
}
.input-form i {
  position: absolute;
  top: 8px;
  left: 0;
  padding: 9px 8px;
  transition: 0.3s;
  color: #aaaaaa;
}
.input-form input[type="text"]:focus + i {
  color: #da3c41;
}

.input-form .tapper {
  max-width: 190px;
  display: inline-flex;
}
.input-form .exp-date {
  min-width: 270px;
  display: inline-flex;
}

.qr-reader {
  padding-left: 10px;
  padding-top: 13px;
  width: 25px;
  height: 25px;
}

.add-button {
  padding-left: 10px;
  padding-top: 10px;
}

header {
  width: 100%;
  height: 70px;
  display: inline-flex;
  background: #009e8c;
}

h1 {
  font-size: 50px;
  padding-left: 20px;
  margin-top: 0;
}

h1::first-letter {
  color: white;
}

.login-info {
  font-size: 20px;
  margin: 0 0 0 auto;
  margin-top: 20px;
  margin-right: 10px;
  text-align: right;
  color: white;
}

.logout-button {
  height: 70px;
  margin-right: 10px;
  margin-left: 10px;
  background: #009e8c; /* ボタン色 */
  border: #009e8c;
  color: white;
  cursor: pointer;
  transition: all 0.3s ease 0s;
}
.logout-button:hover{
  opacity: 0.5;
}
</style>