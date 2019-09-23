import Vue from "vue";
import App from "./App";
import firebase from "firebase/app"
import router from './router.js'
import "firebase/firestore"
import ElementUI from 'element-ui'
import locale from 'element-ui/lib/locale/lang/ja'
import 'element-ui/lib/theme-chalk/index.css'

import { library } from '@fortawesome/fontawesome-svg-core'
import { faCoffee } from '@fortawesome/free-solid-svg-icons'
import { FontAwesomeIcon } from '@fortawesome/vue-fontawesome'

library.add(faCoffee)
Vue.component('font-awesome-icon', FontAwesomeIcon)
Vue.use(ElementUI, {locale})

Vue.config.productionTip = false;

var config = {
  apiKey: "AIzaSyCwKyrL2vUa-WCTLhD7fqJDqh1XbZ7Ek0M",
  authDomain: "frid-vue.firebaseapp.com",
  databaseURL: "https://frid-vue.firebaseio.com",
  projectId: "frid-vue",
  storageBucket: "frid-vue.appspot.com",
  messagingSenderId: "50990339880",
  appId: "1:50990339880:web:cbe60546ac51057f"
};

firebase.initializeApp(config)

new Vue({
  el: '#app',
  router,
  render: h => h(App)
}).$mount("#app");
