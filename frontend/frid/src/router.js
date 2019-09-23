import Vue from 'vue'
import Router from 'vue-router'
import Home from './views/home.vue'
import SignIn from './views/signin.vue'

Vue.use(Router)

const router = new Router({
    routes:[
        {
            path: '/home',
            component: Home,
            meta: { requiredAuth: true}
        },
        {
            path: '/signin',
            component: SignIn
        },
    ]
})

export default router