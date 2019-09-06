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
        <input v-model="exp_date" placeholder="消費期限" />
      </p>
      <button @click="addFoods">追加</button>
      <ul>
        <li v-for="item in list" :key="item.id">
          タッパーID: {{ item.tapper_id }}, 消費期限: {{ item.exp_date }}
          <button
            @click="deleteFoods(item.id)"
          >削除</button>
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
      lastId: 0,
      qr: 2,
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
        .doc(String(this.lastId))
        .set({
          id: this.lastId,
          tapper_id: this.tapper_id,
          exp_date: this.exp_date
        });

      this.tapper_id = null;
      this.exp_date = "";
    },

    // databaseから削除
    deleteFoods(id) {
      firebase
        .firestore()
        .collection("foods")
        .doc(String(id))
        .delete();
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
        this.lastId = buff.length;
      });
  }
};
</script>