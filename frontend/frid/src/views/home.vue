<template>
  <div class="home">
    <div class="main" v-if="hide_qr">
      {{ user.displayName }} でログインしています
      <button @click="signOut">ログアウト</button>
      <p>
        <input v-model="tapper_id" placeholder="タッパーID" />
        <input
          type="image"
          alt="qr"
          src="https://dab1nmslvvntp.cloudfront.net/wp-content/uploads/2017/07/1499401426qr_icon.svg"
          height="20"
          width="20"
          @click="qr_page"
        />
      </p>
      <p>
        <input v-model="food_name" placeholder="食材名" />
      </p>
      <p>
        <input v-model="exp_date" placeholder="消費期限" />
      </p>
      <button @click="addFoods">追加</button>
      <ul>
        <li v-for="item in list" :key="item.tapper_id" v-show="item.is_active">
          <p>タッパーID:{{ item.tapper_id }}</p>
          <p>食材名:{{ item.name }}</p>
          <p>温度:{{ item.temperature }}</p>
          <p>消費期限:{{ item.exp_date }}</p>
          <input
            type="image"
            alt="delete"
            @click="deleteFoods(item.tapper_id)"
            src="https://i.imgur.com/sntj3EF.png"
          />
        </li>
      </ul>
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